<H3DMaterialTemplate Name="PointLight">
    <FxFileName str="PointLight" />
	<UseAutoMatLod value="0" />
    <GLOBAL_VAR varName="HasTransparent" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="ReciveGlobalFog" type="DT_INT" value="1" sem="" />
    <GLOBAL_VAR varName="SortKey" type="DT_INT" value="100" sem="" />
    <GLOBAL_VAR varName="UseTexcoord" type="DT_INT" value="1" sem="" />
    <GLOBAL_VAR varName="bIsTerrain" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bSkeleton" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bUseBumpMap" type="DT_INT" value="1" sem="" />
    <GLOBAL_VAR varName="bUseVertexColor0" type="DT_INT" value="0" sem="" />
    <GLOBAL_VAR varName="bUseVertexColor1" type="DT_INT" value="0" sem="" />
    
    <NODE_VAR varName="PointLightParam" type="DT_FLOAT4" value="20,30,0,0" sem="" />
    <NODE_VAR varName="LightColor" type="DT_FLOAT4" value="1,1,0,0" sem="" />
    <NODE_VAR varName="MapChannel" type="DT_INT" value="0" sem="" />
    <NODE_VAR varName="bUsedForFinalAlphaMap" type="DT_INT" value="0" sem="" />
    <NODE_VAR varName="ProjTexture" type="DT_STRING" value="../data/universal/asset\blank_cube.dds" sem="cubetexturePath" />
    <Function_Code />
    
    <ForwardRenderingSM3_Shader>
   	 	<IncludeFile str='#include &quot;Include\GlobalShaderDefine.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\MatrixDeclarations.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\SamplerState.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\LightFunction.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\SklCalac.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\FogFunction.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\OutDoorLightting.fx&quot;' />
	    <IncludeFile str='#include &quot;Include\prelighting.fx&quot;' />
	    
	    <SpecialShaderConst Semantic="SSCT_WORLD_MATRIX" />
	    <SpecialShaderConst Semantic="SSCT_VIEW_PROJECTION_MATRIX" share="1" />
	    <SpecialShaderConst Semantic="SSCT_VIEWVECTOR" share="1" />
	    <HeaderCode>
	        <HeadCodeAtt name="PointLightParam" type="SC_FLOAT4" semantic="SC_FLOAT" AssoVar="PointLightParam" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
	        <Line str="float4 PointLightParam;" />
	    </HeaderCode>
	     <HeaderCode>
	        <HeadCodeAtt name="LightColor" type="SC_FLOAT4" semantic="SC_FLOAT" AssoVar="LightColor" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
	        <Line str="float4 LightColor;" />
	    </HeaderCode>

    <HeaderCode>
        <HeadCodeAtt name="ProjTexture" type="SC_CUBETEXTURE" semantic="SC_TEXTURE" AssoVar="ProjTexture" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="textureCUBE ProjTexture;" />
        <Line str="samplerCUBE SamplerProjTexture = sampler_state" />
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
            <PSIN_Semantic Semantic="PI_VPOS" />
            <PSIN_Semantic Semantic="PI_TEXCOORD0" />
            <PSIN_Semantic Semantic="PI_TEXCOORD1" />
            <VSMAIN>
                <Line str="VSOUT_0 VSMain_0(VSIN_0 vsin)" />
                <Line str="{" />
                <Line str="&#x09;VSOUT_0 vsout;" InsertPoint="VIP_VS_BEGIN" />
                <Line str="&#x09;float4 TmpPos=float4(vsin.VI_POSITION0.xyz*PointLightParam.y,1.0f);" InsertPoint="VIP_TRANSFORM_WORLDPOS_END" />
                <Line str="&#x09;float4 TmpWorldPos=mul(TmpPos,WorldMatrix);" InsertPoint="VIP_TRANSFORM_WORLDPOS_END" />
                <Line str="&#x09;vsout.VO_POSITION0=mul(TmpWorldPos,ViewProjectionMatrix);" InsertPoint="VIP_TRANSFORM_PROJPOS_END" />
                <Line str="&#x09;vsout.VO_TEXCOORD0=float4(WorldMatrix._41_42_43, 0);" InsertPoint="VIP_VS_COMMONCODE" />
                <Line str="&#x09;vsout.VO_TEXCOORD1=float4(GetEyeDir(vsout.VO_POSITION0), 0);" InsertPoint="VIP_VS_COMMONCODE" />
                <Line str="&#x09;return vsout;" InsertPoint="VIP_TRANSFORM_WORLDPOS_END" />
                <Line str="}" InsertPoint="VIP_VS_END" />
            </VSMAIN>
            <PSMAIN>
                <Line str=" float4 PSMain_0(PSIN_0 psin):COLOR0" />
                <Line str="{" InsertPoint="PIP_PS_BEGIN" />
                <Line str="&#x09;LightPixelShaderMidst midst;" />
								<Line str="&#x09;FillLightPSMidst(midst, psin.PI_VPOS);" />
								<Line str="&#x09;float3 pixelWorldPos = GetWorldFromEyeDir(midst, psin.PI_TEXCOORD1.xyz);" />
								<Line str="&#x09;float3 lightDir = psin.PI_TEXCOORD0.xyz - pixelWorldPos;" />
								<Line str="&#x09;float dist = length(lightDir);" />
								<Line str="&#x09;lightDir /= dist;" />
								<Line str="&#x09;half atten = 1 - smoothstep(PointLightParam.x, PointLightParam.y, dist);" />
								<Line str="&#x09;half3 dnl = saturate(dot(lightDir, midst.normal));" />
								<Line str="&#x09;//float3 transmission = TransmissionColor * midst.transmission;" />
								<Line str="&#x09;//dnl = lerp(dnl, transmission, transmission);" />
								<Line str="&#x09;half3 texProj = 1;" />
								<Line str="&#x09;#if PROJTEXTUREENABL == 1" />
								<Line str="&#x09;texProj = TextureProjectionCUBE(lightDir, WorldMatrix, SamplerProjTexture);" />
								<Line str="&#x09;#endif" />
								
								<Line str="&#x09;half3 light = LightColor.rgb * texProj * dnl * atten;" />
								
								<Line str="&#x09;half3 shadow = 1;" />
								<Line str="&#x09;#if SHADOWENABLE == 1" />
								<Line str="&#x09;shadow = tex2D(Sampler_SSShadowRT0, midst.srCoord).rgb;" />
								<Line str="&#x09;#endif" />
								
								<Line str="&#x09;half3 c0 = light.rgb * shadow.rgb;" />
									
								<Line str="&#x09;half spec = ComputeSpec(midst, lightDir, pixelWorldPos, atten) * LightColor.a;" />
								<Line str="&#x09;return float4(c0, spec);" />
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
