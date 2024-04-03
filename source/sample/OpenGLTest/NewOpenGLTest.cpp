// GaEngine10.cpp : 定义应用程序的入口点。
//
#include "stdheader.h"
#include "Application.h"
#include "Camera.h"
#include "Controller.h"
#include "Model.h"
#include "SceneView.h"
#include "Entity.h"
#include "Scene.h"
#include "SimpleRendering.h"
#include "Platform.h"
#include "Shader.h"


using namespace Aurora;



Entity* CreateSimpleEntity()
{
    auto model = ModelManager.GetModel("resource:Model/White_GrenadeLauncher.model");	
    auto entity =  EntityFactory.CreateEntity(model);
    
    entity->GetTransfrom().Rotate(Quaternionf(Vector3f::UNIT_X, Mathf::PI));
    
    return entity;
}




class OpenGLTest : public Application, public KeyEventListener, public MouseEventListener
{
public:
    OpenGLTest():Application("OpenGLTest")
    {
        
    }
    
    bool		OnInitApp() override;

    void		OnResizeFrameBuffer(int width, int height) override;
    void		OnUpdate(float dt) override;
    void		OnRender() override;

    void	OnKeyEvent(const KeyEvent& event) override;
    void	OnMouseEvent(const MouseEvent& event) override;


    EditorCameraMover m_CamController;
    CameraControllerUE  cameraControl_;

    Entity* m_pEntity;
    
    Camera camera_;

    CScreen*    mMainScreen = nullptr;
    ModelShaderVS mModelShaderVS;
};


bool OpenGLTest::OnInitApp()
{
    mMainScreen = CPlatform::MainScreen;
    mModelShaderVS.InitBase(BaseShader::VERTEX_SHADER, "../dev/data/shader/shader330.vert");
    
    cameraControl_.Transform_.LootAt(Vector3f(0.f, -2.0f, 1.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f::UNIT_Z);

    camera_.SetFov(Mathf::PI / 4.0f);    
        
    GEventDisPatcher->AddKeyEventListener(this);
    GEventDisPatcher->AddMouseEventListener(this);
        
    GEventDisPatcher->AddMouseEventListener(&m_CamController);
    GEventDisPatcher->AddMouseEventListener(&cameraControl_);

    m_pEntity = CreateSimpleEntity();
    

    return true;
    
}

class Visitor : public RenderableVisitor
{
public:
    virtual void Visit(RenderOperator& op)
    {
        op.mVertexFactory = CVertexFactoryPNTT::Instance;
        GRenderDevice->ExecuteOperator(op);
    }
};



void OpenGLTest::OnResizeFrameBuffer(int width, int height)
{
    camera_.SetAspect(width / (float)height);
}


void OpenGLTest::OnUpdate(float dt)
{
    //m_pCamera->SetTransform(m_CamController.GetTransfrom());

    //if (!m_bMultiThread)
    //{
    //	Update();
    //}

    //camera_.Transform_ = m_CamController.Transform_;
    camera_.Transform_ = cameraControl_.Transform_;
}

bool show_demo_window = true;
Color  clearColor;

void OpenGLTest::OnRender()
{
    Visitor visitor;

    // pUpdateView->matView = pCamera->Transform_.Inverse().GetMatrix();
    //
    // pUpdateView->matProj = Matrix4f::PerspectiveFovRH(
    //     pCamera->m_fFov, viewSize.Width / (float)viewSize.Height,
    //     pCamera->GetNearPlane(), pCamera->GetFarPlane());
    //
    // pUpdateView->matViewProj = pUpdateView->matView * pUpdateView->matProj;

    GRenderDevice->Clear(IRenderDevice::CLEAR_FRAME_BUFFER | IRenderDevice::CLEAR_DEPTH_BUFFER, Color(0.2f, 0.2f, 0.2f, 0.2f));
    
        // modelvs_.matWorld = pEntity->mWorld;
        // modelvs_.matWorld.TransposeSelf();

        // modelvs_.mWorldMatrix = pEntity->mWorld;
        //
        // modelvs_.CommitShaderParameter();
        // modelps_.CommitShaderParameter();

    m_pEntity->Accept(visitor);    
}


void OpenGLTest::OnKeyEvent(const KeyEvent& event) 
{
}


void OpenGLTest::OnMouseEvent(const MouseEvent& event)
{
}


OpenGLTest app;