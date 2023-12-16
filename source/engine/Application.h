#ifndef GAAPPLICATION_H
#define GAAPPLICATION_H

#include "stdheader.h"
#include "Timer.h"
#include "Pipeline.h"
#include "Engine.h"

namespace Aurora
{
    class CCanvas
    {
    public:        
        static RectSize GetSize();
    };

    
    class Application
    {
    public:

        static Application* GetCurrentApplication();
        void SwitchApplication();

        
        Application(const string& Name);
        virtual ~Application();


        virtual bool OnInitApp() = 0;

        virtual void OnResizeFrameBuffer(int width, int height)
        {
        }

        virtual void OnUpdate(float dt)
        {
        }

        virtual void OnRender()
        {
        }

        void StartTiming();
        float EndTiming(char* text, bool bTextout = true);

    protected:
        string mName;


        long TimeStart;
        long TimeEnd;

    private:
    };
}


#endif
