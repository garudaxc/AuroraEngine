<H3DMaterialTemplate Name="ScreenSpaceShadow">
    <FxFileName str="ScreenSpaceShadow" />
	<UseAutoMatLod value="0" />
    <GLOBAL_VAR varName="CustomShadow" type="DT_INT" value="1" sem="" />
    <GLOBAL_VAR varName="EffectType" type="DT_STRING" value="Normal" sem="" />
    <GLOBAL_VAR varName="HasTransparent" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="LightMode" type="DT_INT" value="1" sem="" />
    <GLOBAL_VAR varName="ReciveGlobalFog" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="SortKey" type="DT_INT" value="100" sem="" />
    <GLOBAL_VAR varName="UseTexcoord" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bIsTerrain" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bReciveLight" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bReciveShadow" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bSkeleton" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bUseBumpMap" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bUseVertexColor0" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bUseVertexColor1" type="DT_INT" value="0" sem="" />
    <NODE_VAR varName="SceneDepthRT" type="DT_STRING" value="" sem="RT" Desc="" />
    <NODE_VAR varName="ShadowColor" type="DT_FLOAT3" value="0.3,0,0" sem="" Node="" Desc="" />
    <Function_Code />
    <CastShadow_Shader>
        <IncludeFile str='#include &quot;Include\GlobalShaderDefine.fx&quot;' />
        <IncludeFile str='#include &quot;Include\MatrixDeclarations.fx&quot;' />
        <IncludeFile str='#include &quot;Include\SamplerState.fx&quot;' />
        <IncludeFile str='#include &quot;Include\HelperFunction.fx&quot;' />
        <IncludeFile str='#include &quot;Include\SklCalac.fx&quot;' />
        <IncludeFile str='#include &quot;Include\CalcShadow.fx&quot;' />
        <IncludeFile str='#include &quot;Include\prelighting.fx&quot;' />
        <SpecialShaderConst Semantic="SSCT_WORLD_MATRIX" />
        <SpecialShaderConst Semantic="SSCT_SM_VIEWPROJECTION_MATRIX" share="1" />
        <HeaderCode>
            <HeadCodeAtt name="SceneDepthRT" type="SC_RT_TEXTURE" semantic="SC_TEXTURE" AssoVar="SceneDepthRT" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
            <Line str="texture SceneDepthRT;" />
            <Line str="sampler2D Sample_SceneDepthRT = sampler_state" />
            <Line str="{" />
            <Line str="&#x09;Texture=&lt; SceneDepthRT &gt;;" />
            <Line str="&#x09;MinFilter = Point;" />
            <Line str="&#x09;MagFilter = Point;" />
            <Line str="&#x09;MipFilter = Point;" />
            <Line str="&#x09;AddressU  = WRAP;" />
            <Line str="&#x09;AddressV  = WRAP;" />
            <Line str="&#x09;AddressW  = WRAP;" />
 				   	<Line str="&#x09;SRGBTEXTURE = False;" />
            <Line str="};" />
        </HeaderCode>
        <HeaderCode>
            <HeadCodeAtt name="ShadowColor" type="SC_FLOAT3" semantic="SC_FLOAT3" AssoVar="ShadowColor" AssoObject="" AssoObjVar="" share="0" private="1" />
            <Line str="float3 ShadowColor;" />
        </HeaderCode>
        <CASTSHADOW_TECHNIQUE>
            <Pass idx="0">
                <RenderState varName="AlphaBlend" type="DT_INT" value="0" sem="" />
                <RenderState varName="AlphaTest" type="DT_INT" value="0" sem="" />
                <RenderState varName="AlphaTestFunc" type="DT_STRING" value="GreaterEqual" sem="" />
                <RenderState varName="AlphaTestValue" type="DT_INT" value="128" sem="" />
                <RenderState varName="CullMode" type="DT_STRING" value="None" sem="" />
                <RenderState varName="DepthTest" type="DT_INT" value="0" sem="" />
                <RenderState varName="DepthTestFunc" type="DT_STRING" value="LessEqual" sem="" />
                <RenderState varName="DepthWrite" type="DT_INT" value="0" sem="" />
                <RenderState varName="DstBlendFactor" type="DT_STRING" value="InvSrcAlpha" sem="" />
                <RenderState varName="SrcBlendFactor" type="DT_STRING" value="SrcAlpha" sem="" />
                <VSIN_Semantic Semantic="VI_POSITION0" />
                <VSOUT_Semantic Semantic="VO_POSITION0" />
                <VSOUT_Semantic Semantic="VO_TEXCOORD0" />
		            <PSIN_Semantic Semantic="PI_VPOS" />
                <PSIN_Semantic Semantic="PI_TEXCOORD0" />
                <VSMAIN>
                    <Line str="VSOUTCS_0 VSMainCS_0(VSINCS_0 vsin)" />
                    <Line str="{" />
                    <Line str="&#x09;VSOUTCS_0 vsout;" InsertPoint="VIP_VS_BEGIN" />
                    <Line str="&#x09;float4 TmpPos=float4(vsin.VI_POSITION0.xyz,1.0f);" InsertPoint="VIP_VS_TMP_POS" />
                    <Line str="// 结点代码添加" />
                    <Line str="// 结点代码添加结束" />
                    <Line str="&#x09;vsout.VO_POSITION0 = TmpPos;" />
                    <Line str="&#x09;//输出VIEWPORT" />
                    <Line str="&#x09;vsout.VO_TEXCOORD0 = GetViewPort( float2(1,1),BackBufferSizeParam.zw );" />
                    <Line str="&#x09;" InsertPoint="VIP_VS_END" />
                    <Line str="&#x09;return vsout;" />
                    <Line str="}" />
                </VSMAIN>
                <PSMAIN>
                    <Line str="void PSMainCS_0(PSINCS_0 psin, out float4 color : COLOR0 )" />
                    <Line str="{" InsertPoint="PIP_PS_BEGIN" />
                   	<Line str="&#x09;float2 uv = (psin.PI_VPOS + 0.5) * BackBufferSizeParam.xy;" />
					
										<Line str="&#x09;//float depth = tex2D(Sample_SceneDepthRT, uv).x;" />
										<Line str="&#x09;float depth = SampleDepth(Sample_SceneDepthRT, uv);" />
										<Line str="&#x09;float2 newUV = GetRelativePosInVP(psin.PI_VPOS,psin.PI_TEXCOORD0);" />
										<Line str="&#x09;float4 screenQuad = float4(newUV.x, newUV.y, depth, 1);" />
										<Line str="&#x09;float4 worldPos = mul(screenQuad, ViewProjInvMatrix);" />
										<Line str="&#x09;float3 pixelWorldPos = worldPos.xyz / worldPos.w;" />
										<Line str="&#x09;half3 shadow = 1;" />
										<Line str="&#x09;float viewZ = ProjectionMatrix._43 / (depth + ProjectionMatrix._33);" />
										<Line str="&#x09;shadow = CalcShadowDirectional(pixelWorldPos, viewZ, psin.PI_VPOS, ShadowmapProjMatrix);" />
                    <Line str="&#x09;float3 final = lerp(ShadowColor.xyz, float3(1, 1, 1), shadow.x);" />
                    <Line str="&#x09;color = float4(final, 0);" />
                    <Line str="&#x09;" InsertPoint="PIP_PS_END" />
                    <Line str="}" />
                </PSMAIN>
            </Pass>
            <TechniqueCode name="CASTSHADOW_TECHNIQUE" PassNum="1">
                <Line str="technique CASTSHADOW_TECHNIQUE" />
                <Line str="{" />
                <Line str="&#x09;pass Pass_0" />
                <Line str="&#x09;{" />
                <Line str="&#x09;&#x09;VertexShader&#x09;= compile vs_3_0 VSMainCS_0();" />
                <Line str="&#x09;&#x09;PixelShader&#x09;= compile ps_3_0 PSMainCS_0();" />
                <Line str="&#x09;}" />
                <Line str="}" />
            </TechniqueCode>
        </CASTSHADOW_TECHNIQUE>
    </CastShadow_Shader>
    <ForwardRenderingSM3_Shader>
        <IncludeFile str='#include &quot;Include\GlobalShaderDefine.fx&quot;' />
        <IncludeFile str='#include &quot;Include\MatrixDeclarations.fx&quot;' />
        <IncludeFile str='#include &quot;Include\SamplerState.fx&quot;' />
        <IncludeFile str='#include &quot;Include\HelperFunction.fx&quot;' />
        <IncludeFile str='#include &quot;Include\SklCalac.fx&quot;' />
        <IncludeFile str='#include &quot;Include\FogFunction.fx&quot;' />
        <IncludeFile str='#include &quot;Include\OutDoorLightting.fx&quot;' />
        <IncludeFile str='#include &quot;Include\CalcShadow.fx&quot;' />
        <RenderEnvRule name="DCT_NULL" rule="RenderEnvRule_Accept" />
        <SpecialShaderConst Semantic="SSCT_WORLD_MATRIX" />
        <SpecialShaderConst Semantic="SSCT_VIEW_PROJECTION_MATRIX" share="1" />
        <HeaderCode>
            <HeadCodeAtt name="TmpTransparentValue" type="SC_FLOAT" semantic="SC_FLOAT" AssoVar="tmpAlpha" AssoObject="" AssoObjVar="" share="0" private="1" />
            <Line str="float TmpTransparentValue;" />
        </HeaderCode>
        <SHADING_TECHNIQUE>
            <Pass idx="0">
                <RenderState varName="AlphaBlend" type="DT_INT" value="0" sem="" />
                <RenderState varName="AlphaTest" type="DT_INT" value="0" sem="" />
                <RenderState varName="AlphaTestFunc" type="DT_STRING" value="GreaterEqual" sem="" />
                <RenderState varName="AlphaTestValue" type="DT_INT" value="128" sem="" />
                <RenderState varName="CullMode" type="DT_STRING" value="None" sem="" />
                <RenderState varName="DepthTest" type="DT_INT" value="0" sem="" />
                <RenderState varName="DepthTestFunc" type="DT_STRING" value="LessEqual" sem="" />
                <RenderState varName="DepthWrite" type="DT_INT" value="0" sem="" />
                <RenderState varName="DstBlendFactor" type="DT_STRING" value="InvSrcAlpha" sem="" />
                <RenderState varName="SrcBlendFactor" type="DT_STRING" value="SrcAlpha" sem="" />
                <VSIN_Semantic Semantic="VI_POSITION0" />
                <VSOUT_Semantic Semantic="VO_POSITION0" />
                <PSIN_Semantic />
                <VSMAIN>
                    <Line str="VSOUT_0 VSMain_0(VSIN_0 vsin)" />
                    <Line str="{" />
                    <Line str="&#x09;VSOUT_0 vsout;" InsertPoint="VIP_VS_BEGIN" />
                    <Line str="&#x09;float4 TmpPos=float4(vsin.VI_POSITION0.xyz,1.0f);" InsertPoint="VIP_VS_TMP_POS" />
                    <Line str="&#x09;float3 TmpNormal = float3(0,0,1);" InsertPoint="VIP_VS_TMP_NORMAL" />
                    <Line str="&#x09;float3 TmpTangent = float3(1,0,0);" InsertPoint="VIP_VS_TMP_TANGENT" />
                    <Line str="&#x09;float3 TmpBiNormal = float3(0,1,0);" InsertPoint="VIP_VS_TMP_BINORMAL" />
                    <Line str=" " InsertPoint="VIP_VS_ALLTMP_VAR_END" />
                    <Line str="&#x09;float4 TmpWorldPos=mul(TmpPos,WorldMatrix);" InsertPoint="VIP_TRANSFORM_WORLDPOS_END" />
                    <Line str="//结点代码开始" />
                    <Line str="//结点代码结束" />
                    <Line str="&#x09;vsout.VO_POSITION0=mul(TmpWorldPos,ViewProjectionMatrix);" InsertPoint="VIP_TRANSFORM_PROJPOS_END" />
                    <Line str="&#x09;" InsertPoint="VIP_VS_END" />
                    <Line str="&#x09;return vsout;" />
                    <Line str="}" />
                </VSMAIN>
                <PSMAIN>
                    <Line str=" float4 PSMain_0(PSIN_0 psin):COLOR0" />
                    <Line str="{" InsertPoint="PIP_PS_BEGIN" />
                    <Line str="&#x09;float4 PsResult=float4(0,0,0,1);" />
                    <Line str="&#x09;float3 TmpNormal=float3(0,0,0);" />
                    <Line str="&#x09;float TmpAlpha=TmpTransparentValue;" />
                    <Line str="&#x09;float3 TmpDiffuse=float3(0,0,0);" />
                    <Line str="&#x09;float3 TmpSpecluar=float3(0,0,0);" />
                    <Line str="&#x09;float3 TmpSpecluarMask=float3(0,0,0);" />
                    <Line str="&#x09;float TmpSpecluarPower=64.0f;" />
                    <Line str="&#x09;float3 TmpAmibent=float3(0,0,0);" />
                    <Line str="&#x09;float3 TmpEmmit=float3(0,0,0);" />
                    <Line str="&#x09;float3 TmpAnisotropicDir=float3(0,1,0);" />
                    <Line str="&#x09;float3 TmpTransmission= 0; //float3(0,0,0);" />
                    <Line str=" " InsertPoint="PIP_PS_ALLTMP_VAR_END" />
                    <Line str="&#x09;float3 allLightDiffRes=float3(1,1,1 );" />
                    <Line str="&#x09;float3 allLightSpecRes=float3(0,0,0 );" />
                    <Line str="&#x09;float3 TmpWsViewVec=float3(0,0,0 );" />
                    <Line str="// 结点代码开始" />
                    <Line str="&#x09;float3 Constant3Vector_3_Value = float3(0.66,0,0);" />
                    <Line str="&#x09;TmpDiffuse.xyz=Constant3Vector_3_Value;" InsertPoint="PIP_CALC_DIFFUSE_END" />
                    <Line str="// 结点代码结束" InsertPoint="PIP_CALC_MATERIAL_END" />
                    <Line str="&#x09;allLightDiffRes.rgb += GlobalAmibentColor.rgb;" />
                    <Line str="//&#x09;allLightSpecRes *= allLightDiffRes.rgb;&#x09;//" />
                    <Line str="&#x09;PsResult.rgb= allLightDiffRes.rgb*TmpDiffuse.rgb + TmpAmibent.rgb + TmpEmmit.rgb + (TmpSpecluarMask.rgb * (allLightSpecRes.rgb+ TmpSpecluar.rgb));" InsertPoint="PIP_CALC_FINALCOLOR_END" />
                    <Line str="&#x09;PsResult.rgb=float3(0.77, 0, 0);" />
                    <Line str="&#x09;PsResult.a=TmpAlpha;" />
                    <Line str="&#x09;" InsertPoint="PIP_PS_END" />
                    <Line str="&#x09;return PsResult;" />
                    <Line str="}" />
                </PSMAIN>
            </Pass>
            <TechniqueCode name="SHADING_TECHNIQUE" PassNum="1">
                <Line str="technique SHADING_TECHNIQUE" />
                <Line str="{" />
                <Line str="&#x09;pass Pass_0" />
                <Line str="&#x09;{" />
                <Line str="&#x09;&#x09;VertexShader&#x09;= compile vs_3_0 VSMain_0();" />
                <Line str="&#x09;&#x09;PixelShader&#x09;= compile ps_3_0 PSMain_0();" />
                <Line str="&#x09;}" />
                <Line str="}" />
            </TechniqueCode>
        </SHADING_TECHNIQUE>
    </ForwardRenderingSM3_Shader>
</H3DMaterialTemplate>
