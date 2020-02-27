<H3DMaterialTemplate Name="SpotLight">
    <FxFileName str="SpotLight" />
	<UseAutoMatLod value="0" />
    <GLOBAL_VAR varName="HasTransparent" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="ReciveGlobalFog" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="SortKey" type="DT_INT" value="100" sem="" />
    <GLOBAL_VAR varName="UseTexcoord" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bIsTerrain" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bSkeleton" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bUseBumpMap" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bUseVertexColor0" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bUseVertexColor1" type="DT_INT" value="0" sem="" />

    <Function_Code />
    
    <NODE_VAR varName="SpotLightParam" type="DT_FLOAT4" value="1,0.707,0.866,6.288" sem="" />
    <NODE_VAR varName="LightRange" type="DT_FLOAT4" value="100,100,0,0" sem="" />
    <NODE_VAR varName="LightColor" type="DT_FLOAT4" value="1,1,0,0" sem="" />
    <NODE_VAR varName="ShadowColor" type="DT_FLOAT4" value="0,0,0,0" sem="" />
    <NODE_VAR varName="ShadowProjMatrix" type="DT_MATRIX44" value="0,0,0,0;0,0,0,0;0,0,0,0;0,0,0,0;" sem="" />
    <NODE_VAR varName="TexProjMatrix" type="DT_MATRIX44" value="0,0,0,0;0,0,0,0;0,0,0,0;0,0,0,0;" sem="" />
    <NODE_VAR varName="MapChannel" type="DT_INT" value="0" sem="" />
    <NODE_VAR varName="bUsedForFinalAlphaMap" type="DT_INT" value="0" sem="" />
    <NODE_VAR varName="ProjTexture" type="DT_STRING" value="../data/universal/asset\blank_2d.dds" sem="texturePath" />
    <ForwardRenderingSM3_Shader>
    <IncludeFile str='#include &quot;Include\GlobalShaderDefine.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\MatrixDeclarations.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\SamplerState.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\LightFunction.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\OutDoorLightting.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\CalcShadow.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\prelighting.fx&quot;' />
	
	    <SpecialShaderConst Semantic="SSCT_WORLD_MATRIX" />
	    <SpecialShaderConst Semantic="SSCT_VIEW_PROJECTION_MATRIX" share="1" />
	    <SpecialShaderConst Semantic="SSCT_VIEWVECTOR" share="1" />
	    <HeaderCode>
	    		<"// x : tangent of half outter angle"/>
					<"// y : cosine of half outter angle"/>
					<"// z : 1 / (cosine of half inner angle - y)"/>
					<"// w : far range of the light"/>
	        <HeadCodeAtt name="SpotLightParam" type="SC_FLOAT4" semantic="SC_FLOAT" AssoVar="SpotLightParam" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
	        <Line str="float4 SpotLightParam;" />
	    </HeaderCode>
	    <HeaderCode>
	        <HeadCodeAtt name="LightColor" type="SC_FLOAT4" semantic="SC_FLOAT" AssoVar="LightColor" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
	        <Line str="float4 LightColor;" />
	    </HeaderCode>
	    <HeaderCode>
	        <HeadCodeAtt name="LightRange" type="SC_FLOAT4" semantic="SC_FLOAT" AssoVar="LightRange" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
	        <Line str="float4 LightRange;" />
	    </HeaderCode>
    <HeaderCode>
        <HeadCodeAtt name="ShadowColor" type="SC_FLOAT4" semantic="SC_FLOAT" AssoVar="ShadowColor" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="float4 ShadowColor;" />
    </HeaderCode>
    <HeaderCode>
        <HeadCodeAtt name="ShadowProjMatrix" type="SC_NODEVAR_MATRIX4" semantic="SC_FLOAT" AssoVar="ShadowProjMatrix" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="float4x4 ShadowProjMatrix;" />
    </HeaderCode>
    <HeaderCode>
        <HeadCodeAtt name="TexProjMatrix" type="SC_NODEVAR_MATRIX4" semantic="SC_FLOAT" AssoVar="TexProjMatrix" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="float4x4 TexProjMatrix;" />
    </HeaderCode>
    <HeaderCode>
        <HeadCodeAtt name="ProjTexture" type="SC_TEXTURE" semantic="SC_TEXTURE" AssoVar="ProjTexture" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="texture ProjTexture;" />
        <Line str="sampler2D SamplerProjTexture = sampler_state" />
        <Line str="{" />
        <Line str="&#x09;Texture=&lt; ProjTexture &gt;;" />
        <Line str="&#x09;MinFilter = LINEAR;" />
        <Line str="&#x09;MagFilter = LINEAR;" />
        <Line str="&#x09;MipFilter = LINEAR;" />
        <Line str="&#x09;AddressU  = WRAP;" />
        <Line str="&#x09;AddressV  = WRAP;" />
        <Line str="&#x09;AddressW  = WRAP;" />
					            <Line str="&#x09;MipLODBias = 0;" />
            <Line str="&#x09;MaxAnisotropy = 0;" />
            <Line str="&#x09;SRGBTEXTURE = FALSE;" />
        <Line str="};" />
    </HeaderCode>
       
    <SHADING_TECHNIQUE>
        <Pass idx="0">
            <RenderState varName="CullMode" type="DT_STRING" value="CCW" sem="" />
            <RenderState varName="DepthTest" type="DT_INT" value="1" sem="" />
            <RenderState varName="DepthTestFunc" type="DT_STRING" value="Greater" sem="" />
            <RenderState varName="DepthWrite" type="DT_INT" value="0" sem="" />
            <RenderState varName="AlphaTest" type="DT_INT" value="0" sem="" />
        		<RenderState varName="AlphaBlend" type="DT_INT" value="1" sem="" />
            <RenderState varName="DstBlendFactor" type="DT_STRING" value="One" sem="" />
            <RenderState varName="SrcBlendFactor" type="DT_STRING" value="One" sem="" />     
            <VSIN_Semantic Semantic="VI_POSITION0" />
            <VSOUT_Semantic Semantic="VO_POSITION0" />
            <VSOUT_Semantic Semantic="VO_TEXCOORD0" />
            <VSOUT_Semantic Semantic="VO_TEXCOORD1" />
            <VSOUT_Semantic Semantic="VO_TEXCOORD2" />
            <PSIN_Semantic Semantic="PI_VPOS" />
            <PSIN_Semantic Semantic="PI_TEXCOORD0" />
            <PSIN_Semantic Semantic="PI_TEXCOORD1" />
            <PSIN_Semantic Semantic="PI_TEXCOORD2" />
            <VSMAIN>
                <Line str="VSOUT_0 VSMain_0(VSIN_0 vsin)" />
                <Line str="{" />
                <Line str="&#x09;VSOUT_0 vsout;" InsertPoint="VIP_VS_BEGIN" />
                <Line str="&#x09;float len = SpotLightParam.w;" />
                <Line str="&#x09;float2 radius = vsin.VI_POSITION0.xz * SpotLightParam.x * len;" />
								<Line str="&#x09;float3 objPos = float3(radius.x, vsin.VI_POSITION0.y * len, radius.y);" />
                <Line str="&#x09;float4 TmpPos=float4(objPos,1.0f);" InsertPoint="VIP_TRANSFORM_WORLDPOS_END" />
                <Line str="&#x09;float4 TmpWorldPos=mul(TmpPos,WorldMatrix);" InsertPoint="VIP_TRANSFORM_WORLDPOS_END" />
                <Line str="&#x09;vsout.VO_POSITION0=mul(TmpWorldPos,ViewProjectionMatrix);" InsertPoint="VIP_TRANSFORM_PROJPOS_END" />
                <Line str="&#x09;//默认方向是y轴负方向" />
                <Line str="&#x09;vsout.VO_TEXCOORD0=float4(-WorldMatrix._21_22_23, 0);" InsertPoint="VIP_VS_COMMONCODE" />
                <Line str="&#x09;vsout.VO_TEXCOORD1=float4(WorldMatrix._41_42_43, 0);" InsertPoint="VIP_VS_COMMONCODE" />
                <Line str="&#x09;vsout.VO_TEXCOORD2=float4(GetEyeDir(vsout.VO_POSITION0), 0);" InsertPoint="VIP_VS_COMMONCODE" />
                <Line str="&#x09;return vsout;" InsertPoint="VIP_TRANSFORM_WORLDPOS_END" />
                <Line str="}" InsertPoint="VIP_VS_END" />
            </VSMAIN>
            <PSMAIN>
                <Line str=" float4 PSMain_0(PSIN_0 psin):COLOR0" />
                <Line str="{" InsertPoint="PIP_PS_BEGIN" />
                                
                <Line str="&#x09;LightPixelShaderMidst midst;" />
								<Line str="&#x09;FillLightPSMidst(midst, psin.PI_VPOS);" />
								<Line str="&#x09;float3 pixelWorldPos = GetWorldFromEyeDir(midst, psin.PI_TEXCOORD2.xyz);" />
								<Line str="&#x09;float3 tempLight = psin.PI_TEXCOORD1.xyz - pixelWorldPos;" />
								<Line str="&#x09;float lightDist = length(tempLight);" />
								<Line str="&#x09;float3 lightDir = tempLight / lightDist;" />
								<Line str="&#x09;half3 dnl = saturate(dot(lightDir, midst.normal));" />
								
								<Line str="&#x09;//half3 transmission = TransmissionColor * midst.transmission;" />
								<Line str="&#x09;//dnl = lerp(dnl, transmission, transmission);" />
									
								<Line str="&#x09;half angleCos = dot(lightDir, psin.PI_TEXCOORD0.xyz);" />
								<Line str="&#x09;half atten  = saturate((angleCos - SpotLightParam.y) * SpotLightParam.z);" />
								<Line str="&#x09;atten = atten * atten;" />
								<Line str="&#x09;atten *= (1 - smoothstep(LightRange.x, LightRange.y, lightDist));" />
								<Line str="&#x09;// clip very dark pixel" />
								<Line str="&#x09;clip(atten - 0.001);" />
								<Line str="&#x09;half4 texProj = 1;" />
								<Line str="&#x09;#if PROJTEXTUREENABL == 1" />
								<Line str="&#x09;texProj = half4(TextureProjection2D(pixelWorldPos, TexProjMatrix, SamplerProjTexture), 0);" />
								<Line str="&#x09;#endif" />
								<Line str="&#x09;half3 light = LightColor.rgb * texProj.rgb * dnl * atten;" />
								<Line str="&#x09;half spec = ComputeSpec(midst, lightDir, pixelWorldPos, atten) * LightColor.a;" />
								<Line str="&#x09;half3 shadow = 1;" />
								
								<Line str="&#x09;#if SHADOWENABLE == 1" />
								<Line str="&#x09;shadow = tex2D(Sampler_SSShadowRT0, midst.srCoord).rgb;" />
								<Line str="&#x09;#endif" />
								<Line str="&#x09;half3 c0 = light.rgb * shadow.rgb;" />
								<Line str="&#x09;return float4(c0, spec * texProj.a);" />
                <Line str="}" InsertPoint="PIP_PS_END" />
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
