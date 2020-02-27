<H3DMaterialTemplate Name="DirectionalLight">
    <FxFileName str="DirectionalLight" />
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
	  <NODE_VAR varName="LightColor" type="DT_FLOAT4" value="1,1,0,0" sem="" />
    <NODE_VAR varName="ShadowColor" type="DT_FLOAT4" value="0,0,0,0" sem="" />
    <NODE_VAR varName="ShadowProjMatrix0" type="DT_MATRIX44" value="0,0,0,0;0,0,0,0;0,0,0,0;0,0,0,0;" sem="" />
    <NODE_VAR varName="ShadowProjMatrix1" type="DT_MATRIX44" value="0,0,0,0;0,0,0,0;0,0,0,0;0,0,0,0;" sem="" />
    <NODE_VAR varName="ShadowProjMatrix2" type="DT_MATRIX44" value="0,0,0,0;0,0,0,0;0,0,0,0;0,0,0,0;" sem="" />
	  <NODE_VAR varName="MapChannel" type="DT_INT" value="0" sem="" />
	  <NODE_VAR varName="bUsedForFinalAlphaMap" type="DT_INT" value="0" sem="" />
	    
    <Function_Code />
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
	        <HeadCodeAtt name="LightColor" type="SC_FLOAT4" semantic="SC_FLOAT" AssoVar="LightColor" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
	        <Line str="float4 LightColor;" />
	    </HeaderCode>
	     <HeaderCode>
        <HeadCodeAtt name="ShadowColor" type="SC_FLOAT4" semantic="SC_FLOAT" AssoVar="ShadowColor" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="float4 ShadowColor;" />
    </HeaderCode>
    <HeaderCode>
        <HeadCodeAtt name="ShadowProjMatrix0" type="SC_NODEVAR_MATRIX4" semantic="SC_FLOAT" AssoVar="ShadowProjMatrix0" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="float4x4 ShadowProjMatrix0;" />
    </HeaderCode>
    <HeaderCode>
        <HeadCodeAtt name="ShadowProjMatrix1" type="SC_NODEVAR_MATRIX4" semantic="SC_FLOAT" AssoVar="ShadowProjMatrix1" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="float4x4 ShadowProjMatrix1;" />
    </HeaderCode>
    <HeaderCode>
        <HeadCodeAtt name="ShadowProjMatrix2" type="SC_NODEVAR_MATRIX4" semantic="SC_FLOAT" AssoVar="ShadowProjMatrix2" AssoObject="" AssoObjVar="" share="0" private="1" FinalAlphaMap="0" />
        <Line str="float4x4 ShadowProjMatrix2;" />
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
	                <Line str="&#x09;vsout.VO_POSITION0=float4(vsin.VI_POSITION0.xyz, 1);" InsertPoint="VIP_TRANSFORM_PROJPOS_END" />
	                <Line str="&#x// light direction is objec's minus y axis" />
	                <Line str="&#x09;vsout.VO_TEXCOORD0=float4(-WorldMatrix._21_22_23, 0);" InsertPoint="VIP_VS_COMMONCODE" />
	                <Line str="&#x09;vsout.VO_TEXCOORD1=float4(GetEyeDir(vsout.VO_POSITION0), 0);" InsertPoint="VIP_VS_COMMONCODE" />
	                <Line str="&#x09;return vsout;" InsertPoint="VIP_TRANSFORM_WORLDPOS_END" />
	                <Line str="}" InsertPoint="VIP_VS_END" />
	            </VSMAIN>
	            <PSMAIN>
	                <Line str=" float4 PSMain_0(PSIN_0 psin):COLOR0" />
	                <Line str="{" InsertPoint="PIP_PS_BEGIN" />
	                <Line str="&#x09;LightPixelShaderMidst midst;" />
									<Line str="&#x09;FillLightPSMidst(midst, psin.PI_VPOS);" />
									<Line str="&#x09;float3 lightDir = psin.PI_TEXCOORD0.xyz;" />
									<Line str="&#x09;float3 pixelWorldPos = GetWorldFromEyeDir(midst, psin.PI_TEXCOORD1.xyz);" />
									<Line str="&#x09;half3 dnl = saturate(dot(lightDir, midst.normal));" />
									
									<Line str="&#x09;//float3 transmission = TransmissionColor * midst.transmission;" />
									<Line str="&#x09;//dnl = lerp(dnl, transmission, transmission);" />
									
									<Line str="&#x09;half3 light = LightColor.rgb * dnl;" />
									<Line str="&#x09;half spec = ComputeSpec(midst, lightDir, pixelWorldPos, 1) * LightColor.a;" />
									<Line str="&#x09;half3 shadow = 1;" />
									<Line str="&#x09;#if SHADOWENABLE == 1" />
									<Line str="&#x09;float4x4 shadowProjMatrix[3] = {ShadowProjMatrix0, ShadowProjMatrix1, ShadowProjMatrix2};" />
									<Line str="&#x09;//shadow = CalcShadowDirectional(pixelWorldPos, midst.viewZ, midst.vPos, shadowProjMatrix);" />
									<Line str="&#x09;shadow = tex2D(Sampler_SSShadowRT0, midst.srCoord);" />
									<Line str="&#x09;#endif" />
									<Line str="&#x09;//half3 c0 = lerp(ShadowColor.rgb, light, shadow.r);" />
									<Line str="&#x09;half3 c0 = light.rgb * shadow.rgb;" />
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
