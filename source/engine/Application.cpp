#include "stdHeader.h"
#include <time.h>
#include "Application.h"
#include "Console.h"



namespace Aurora
{

    vector<Application*>    GApplications;


    Application* Application::GetCurrentApplication()
    {
        if(GApplications.size() > 0)
        {
            return GApplications[0];
        }

        return nullptr;        
    }

    void Application::SwitchApplication()
    {
    }

    Application::Application(const string& Name)
    {
        mName = Name;
        GApplications.push_back(this);
    }


    Application::~Application()
    {
    }


    void Application::StartTiming()
    {
        TimeStart = clock();
    }

    float Application::EndTiming(char* text, bool bTextout)
    {
        TimeEnd = clock();

        float elepsed = (TimeEnd - TimeStart) / (float)CLOCKS_PER_SEC;
        if (bTextout)
        {
            std::stringstream ss;
            ss << text << " : " << elepsed << std::endl;
            Console::AppendText(ss.str().c_str());
        }

        return elepsed;
    }

    extern void EngineWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
