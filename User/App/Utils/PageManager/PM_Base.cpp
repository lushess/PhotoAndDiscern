/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "PageManager.h"
#include "PM_Log.h"
#include <algorithm>
#include "FreeRTOS.h"
#include "queue.h"

#define PM_EMPTY_PAGE_NAME "EMPTY_PAGE"

PageBase::PageBase()
			:_root(NULL)
{}
PageBase::~PageBase()
{}
	
/**
  * @brief  Page manager constructor
  * @param  factory: Pointer to the page factory
  * @retval None
  */
PageManager::PageManager(PageFactory* factory)
    : _Factory(factory)
    , _PagePrev(NULL)
    , _PageCurrent(NULL)
    , _RootDefaultStyle(NULL)
		, swMutex(xSemaphoreCreateMutex())
		, swQueue(xQueueCreate(5,sizeof(Switch_Info_t)))
    
{
    memset(&_AnimState, 0, sizeof(_AnimState));
    memset(&switchinfo,NULL,sizeof(switchinfo));

    SetGlobalLoadAnimType();		
}

/**
  * @brief  Page manager destructor
  * @param  None
  * @retval None
  */
PageManager::~PageManager()
{
    SetStackClear(); 
		if (TaskSwitchTo_Handle) {
        vTaskDelete(TaskSwitchTo_Handle); //Delete Page Switch Task
				TaskSwitchTo_Handle = NULL;
    }
		vSemaphoreDelete(swMutex);
		vQueueDelete(swQueue);
}

/**
  * @brief  Search pages in the page pool
  * @param  name: Page name
  * @retval A pointer to the base class of the page, or NULL if not found
  */
PageBase* PageManager::FindPageInPool(const char* name)
{
    for (auto iter : _PagePool)
    {
        if (strcmp(name, iter->_Name) == 0)
        {
            return iter;
        }
    }
    return NULL;
}

/**
  * @brief  Search pages in the page stack
  * @param  name: Page name
  * @retval A pointer to the base class of the page, or NULL if not found
  */
PageBase* PageManager::FindPageInStack(const char* name)
{
    decltype(_PageStack) stk = _PageStack;
    while (!stk.empty())
    {
        PageBase* base = stk.top();

        if (strcmp(name, base->_Name) == 0)
        {
            return base;
        }

        stk.pop();
    }

    return NULL;
}

/**
  * @brief  Install the page, and register the page to the page pool
  * @param  className: The class name of the page
  * @param  appName: Page application name, no duplicates allowed
  * @retval Return true if successful
  */
bool PageManager::Install(const char* className, const char* appName)
{
		if(TaskSwitchTo_Handle == NULL) TaskSwitchToCreate();//Create Page Switch Task

    if (_Factory == NULL)
    {
        PM_LOG_ERROR("Factory was not registered, can't install page");
        return false;
    }

    if (appName == NULL)
    {
        PM_LOG_WARN("appName has not set");
        appName = className;
    }

    if (FindPageInPool(appName) != NULL)
    {
        PM_LOG_ERROR("Page(%s) was registered", appName);
        return false;
    }

    PageBase* base = _Factory->CreatePage(className);
    if (base == NULL)
    {
        PM_LOG_ERROR("Factory has not %s", className);
        return false;
    }

    base->_root = NULL;
    base->_ID = 0;
    base->_Manager = NULL;
    base->_UserData = NULL;
    memset(&base->priv, 0, sizeof(base->priv));

    PM_LOG_INFO("Install Page[class = %s, name = %s]", className, appName);
		PM_LOG_INFO("Page AddressBase:%p", base);
		
    bool retval = Register(base, appName);

    base->onCustomAttrConfig();
		
    return retval;
}

/**
  * @brief  Uninstall page
  * @param  appName: Page application name, no duplicates allowed
  * @retval Return true if the uninstallation is successful
  */
bool PageManager::Uninstall(const char* appName)
{
    PM_LOG_INFO("Page(%s) uninstall...", appName);

    vTaskDelete(TaskSwitchTo_Handle);

    PageBase* base = FindPageInPool(appName);
    if (base == NULL)
    {
        PM_LOG_ERROR("Page(%s) was not found", appName);
        return false;
    }

    if (!Unregister(appName))
    {
        PM_LOG_ERROR("Page(%s) unregister failed", appName);
        return false;
    }

    if (base->priv.IsCached)
    {
        PM_LOG_WARN("Page(%s) has cached, unloading...", appName);
        base->priv.State = PageBase::PAGE_STATE_UNLOAD;
        StateUpdate(base);
    }
    else
    {
        PM_LOG_INFO("Page(%s) has not cache", appName);
    }

    delete base;
    PM_LOG_INFO("Uninstall OK");
    return true;
}

/**
  * @brief  Register the page to the page pool
  * @param  name: Page application name, duplicate registration is not allowed
  * @retval Return true if the registration is successful
  */
bool PageManager::Register(PageBase* base, const char* name)
{
    if (FindPageInPool(name) != NULL)
    {
        PM_LOG_ERROR("Page(%s) was multi registered", name);
        return false;
    }

    base->_Manager = this;
    base->_Name = name;

    _PagePool.push_back(base);

    return true;
}

/**
  * @brief  Log out the page from the page pool
  * @param  name: Page application name
  * @retval Return true if the logout is successful
  */
bool PageManager::Unregister(const char* name)
{
    PM_LOG_INFO("Page(%s) unregister...", name);

    PageBase* base = FindPageInStack(name);

    if (base != NULL)
    {
        PM_LOG_ERROR("Page(%s) was in stack", name);
        return false;
    }

    base = FindPageInPool(name);
    if (base == NULL)
    {
        PM_LOG_ERROR("Page(%s) was not found", name);
        return false;
    }

    auto iter = std::find(_PagePool.begin(), _PagePool.end(), base);

    if (iter == _PagePool.end())
    {
        PM_LOG_ERROR("Page(%s) was not found in PagePool", name);
        return false;
    }

    _PagePool.erase(iter);

    PM_LOG_INFO("Unregister OK");
    return true;
}

/**
  * @brief  Get the top page of the page stack
  * @param  None
  * @retval A pointer to the base class of the page
  */
PageBase* PageManager::GetStackTop()
{
    return _PageStack.empty() ? NULL : _PageStack.top();
}

/**
  * @brief  Get the page below the top of the page stack
  * @param  None
  * @retval A pointer to the base class of the page
  */
PageBase* PageManager::GetStackTopAfter()
{
    PageBase* top = GetStackTop();

    if (top == NULL)
    {
        return NULL;
    }

    _PageStack.pop();

    PageBase* topAfter = GetStackTop();

    _PageStack.push(top);

    return topAfter;
}

/**
  * @brief  Clear the page stack and end the life cycle of all pages in the page stack
  * @param  keepBottom: Whether to keep the bottom page of the stack
  * @retval None
  */
void PageManager::SetStackClear(bool keepBottom)
{
    while (1)
    {
        PageBase* top = GetStackTop();

        if (top == NULL)
        {
            PM_LOG_INFO("Page stack is empty, breaking...");
            break;
        }

        PageBase* topAfter = GetStackTopAfter();

        if (topAfter == NULL)
        {
            if (keepBottom)
            {
                _PagePrev = top;
                PM_LOG_INFO("Keep page stack bottom(%s), breaking...", top->_Name);
                break;
            }
            else
            {
                _PagePrev = NULL;
            }
        }

        FourceUnload(top);

        _PageStack.pop();
    }
    PM_LOG_INFO("Stack clear done");
}

/**
  * @brief  Get the name of the previous page
  * @param  None
  * @retval The name of the previous page, if it does not exist, return PM_EMPTY_PAGE_NAME
  */
const char* PageManager::GetPagePrevName()
{
    return _PagePrev ? _PagePrev->_Name : PM_EMPTY_PAGE_NAME;
}

/**
  * @brief  Get the pointer of the previous page
  * @param  None
  * @retval The pointer of the previous page, if it does not exist, return NULL
  */
PageBase* PageManager::GetPagePrevPointer()
{
    return _PagePrev;
}

/**
  * @brief  Get the pointer of the current page
  * @param  None
  * @retval The pointer of the current page, if it does not exist, return NULL
  */
PageBase* PageManager::GetCurrentPagePointer()
{
    return _PageCurrent;
}

