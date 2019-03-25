#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Develop/DebugRenderer.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderTypes.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"

STATIC DebugRenderer* DebugRenderer::s = nullptr;

////////////////////////////////
DebugRenderObject::DebugRenderObject(RenderContext* renderer)
{
	m_cpuMesh = new CPUMesh();
	m_gpuMesh = new GPUMesh(renderer);
}


////////////////////////////////
DebugRenderObject::~DebugRenderObject()
{
	delete m_cpuMesh;
	delete m_gpuMesh;
}

////////////////////////////////
STATIC void DebugRenderer::Startup(RenderContext* renderer)
{
	if (s != nullptr) {
		return;
	}
	s = new DebugRenderer(renderer);
}

////////////////////////////////
STATIC void DebugRenderer::Shutdown()
{
	if (s == nullptr) {
		return;
	}
	delete s;
	s = nullptr;
}

////////////////////////////////
void DebugRenderer::BeginFrame()
{

}

////////////////////////////////
STATIC void DebugRenderer::Update(float deltaSeconds)
{
	if (s == nullptr)
		return;
	auto& worldObjects = s->m_WorldObjects;
	auto& screenObjects = s->m_screenObjects;

	// Remove all expired objects
	for (auto iter = worldObjects.begin(); iter != worldObjects.end();) {
		auto currentObj = *iter;
		currentObj->m_upTime += deltaSeconds;
		if (currentObj->m_lifeTime >= 0.f) {
			if (currentObj->m_upTime > currentObj->m_lifeTime) {
				delete currentObj;
				iter = worldObjects.erase(iter);
				continue;
			}
		}
		++iter;
	}
	for (auto iter = screenObjects.begin(); iter != screenObjects.end();) {
		auto currentObj = *iter;
		currentObj->m_upTime += deltaSeconds;
		if (currentObj->m_lifeTime >= 0.f) {
			if (currentObj->m_upTime > currentObj->m_lifeTime) {
				delete currentObj;
				iter = screenObjects.erase(iter);
				continue;
			}
		}
		++iter;
	}


	// Update color
	for (auto eachObj : worldObjects) {
		if (eachObj->m_lifeTime < 0.f) {
			continue;
		}
		CPUMesh* mesh = eachObj->m_cpuMesh;
		Rgba color = eachObj->m_colorGradient.GetColorAt(eachObj->m_upTime / eachObj->m_lifeTime);
		int vertexCount = mesh->GetVertexCount();
		for (int vid = 0; vid < vertexCount; ++vid) {
			mesh->SetVertexColorByIndex(vid, color);
		}
	}

	for (auto eachObj : screenObjects) {
		if (eachObj->m_lifeTime < 0.f) {
			continue;
		}
		CPUMesh* mesh = eachObj->m_cpuMesh;
		Rgba color = eachObj->m_colorGradient.GetColorAt(eachObj->m_upTime / eachObj->m_lifeTime);
		int vertexCount = mesh->GetVertexCount();
		for (int vid = 0; vid < vertexCount; ++vid) {
			mesh->SetVertexColorByIndex(vid, color);
		}
	}
}

////////////////////////////////
STATIC void DebugRenderer::Render(Camera* worldCamera)
{
	if (s == nullptr)
		return;
	if (!s->m_isRendering)
		return;
	RenderContext* renderer = s->m_renderer;
	Shader* shader = s->m_shader;
	
	// On World Objects
	shader->ResetShaderStates();
	RenderTargetView* renderTarget = renderer->GetFrameColorTarget();
	worldCamera->SetRenderTarget(renderTarget);
	DepthStencilTargetView* dstTarget = renderer->GetFrameDepthStencilTarget();
	worldCamera->SetDepthStencilTarget(dstTarget);

	renderer->BeginCamera(*worldCamera);

	ConstantBuffer* rendererModel = renderer->GetModelBuffer();
	Mat4 cameraRotation = worldCamera->GetCameraModel();
	cameraRotation[Tx] = 0;
	cameraRotation[Ty] = 0;
	cameraRotation[Tz] = 0;
	cameraRotation[Tw] = 1;
	std::vector<DebugRenderObject*>& worldObjects = s->m_WorldObjects;
	for (auto eachObject : worldObjects) {
		eachObject->m_gpuMesh->CopyFromCPUMesh(*(eachObject->m_cpuMesh));
	}
	// X-ray Pre render
	shader->SetBlendMode(BLEND_MODE_ADDITIVE);
	for (auto eachObject : worldObjects) {
		if (eachObject->m_renderMode != DEBUG_RENDER_XRAY) {
			continue;
		}
		//Vec3 newOrientation = cameraPosition - eachObject->m_position;
		Mat4 model = Mat4::MakeTranslate3D(eachObject->m_position);
		if (eachObject->m_isBillboard) {
			model *= cameraRotation;
			/*DebuggerPrintf("%g %g %g\n", roatation.x, roatation.y, roatation.z);*/
		}
		renderer->BindConstantBuffer(CONSTANT_SLOT_MODEL, rendererModel);
		rendererModel->Buffer(&model, sizeof(model));
		renderer->BindTextureViewWithSampler(0, eachObject->m_texture);
		shader->UseState(s->m_dsXray, s->GetRasterizerState(eachObject->m_rasterizeMode));
		renderer->DrawMesh(*(eachObject->m_gpuMesh));
	}
	// Normal rendering
	shader->SetBlendMode(BLEND_MODE_ALPHA);
	for (auto eachObject : worldObjects) {
		if (eachObject->m_isTransparent) {
			continue;
		}
		//Vec3 newOrientation = cameraPosition - eachObject->m_position;
		Mat4 model = Mat4::MakeTranslate3D(eachObject->m_position);
		if (eachObject->m_isBillboard) {
			model *= cameraRotation;
			/*DebuggerPrintf("%g %g %g\n", roatation.x, roatation.y, roatation.z);*/
		}
		renderer->BindConstantBuffer(CONSTANT_SLOT_MODEL, rendererModel);
		rendererModel->Buffer(&model, sizeof(model));
		renderer->BindTextureViewWithSampler(0, eachObject->m_texture);
		shader->UseState(s->m_dsUseDepth, s->GetRasterizerState(eachObject->m_rasterizeMode));
		renderer->DrawMesh(*(eachObject->m_gpuMesh));
	}
	// AlwaysMode After renfering
	for (auto eachObject : worldObjects) {
		if (eachObject->m_renderMode == DEBUG_RENDER_ALWAYS || eachObject->m_isTransparent == true) {
			Mat4 model = Mat4::MakeTranslate3D(eachObject->m_position);
			if (eachObject->m_isBillboard) {
				model *= cameraRotation;
				/*DebuggerPrintf("%g %g %g\n", roatation.x, roatation.y, roatation.z);*/
			}
			renderer->BindConstantBuffer(CONSTANT_SLOT_MODEL, rendererModel);
			rendererModel->Buffer(&model, sizeof(model));
			renderer->BindTextureViewWithSampler(0, eachObject->m_texture);
			shader->UseState(
				eachObject->m_isTransparent ? s->m_dsTransparent : s->m_dsAlways
				, s->GetRasterizerState(eachObject->m_rasterizeMode));
			renderer->DrawMesh(*(eachObject->m_gpuMesh));
		}
	}

	renderer->EndCamera(*worldCamera);

	//////////////////////////////////////////////////////////////////////////
	//On Screen Objects
	shader->ResetShaderStates();
	Camera* screenCamera = s->m_screenCamera;
	screenCamera->SetRenderTarget(renderTarget);
	screenCamera->SetDepthStencilTarget(dstTarget);
	renderer->BeginCamera(*screenCamera);
	rendererModel->Buffer(&Mat4::Identity, sizeof(Mat4));
	for (auto eachObject : s->m_screenObjects) {
		eachObject->m_gpuMesh->CopyFromCPUMesh(*(eachObject->m_cpuMesh));
		shader->UseState(s->m_dsAlways, s->GetRasterizerState(DEBUG_RENDER_FILL));
		renderer->BindConstantBuffer(CONSTANT_SLOT_MODEL, rendererModel);
		renderer->BindTextureViewWithSampler(0, eachObject->m_texture);
		renderer->DrawMesh(*(eachObject->m_gpuMesh));
	}
	renderer->EndCamera(*screenCamera);

}

////////////////////////////////
void DebugRenderer::EndFrame()
{

}

////////////////////////////////
void DebugRenderer::Clear()
{
	for (auto each : s->m_WorldObjects)
		delete each;
	for (auto each : s->m_screenObjects)
		delete each;
	s->m_WorldObjects.clear();
	s->m_screenObjects.clear();
}

////////////////////////////////
void DebugRenderer::ToggleRendering(bool isRendering)
{
	if (s == nullptr)
		return;
	s->m_isRendering = isRendering;
}

////////////////////////////////
void DebugRenderer::ToggleRendering()
{
	if (s == nullptr)
		return;
	s->m_isRendering = !s->m_isRendering;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawPoint3D(const Vec3& position, float size, float time/*=-1.f*/, const RgbaGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* point = new DebugRenderObject(s->m_renderer);
	point->m_position = position;
	point->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	point->m_cpuMesh->AddAABB2ToMesh(AABB2::MakeAABB2OfSize(size, size));
	point->m_lifeTime = time;
	point->m_colorGradient = colorGradient;
	point->m_isBillboard = true;
	s->m_WorldObjects.push_back(point);
	return point;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawLine3D(const Vec3& start, const Vec3& end, float thickness, float time/*=-1.f*/, const RgbaGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* line = new DebugRenderObject(s->m_renderer);
	line->m_position = start;
	line->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	line->m_cpuMesh->AddCylinderToMesh(Vec3::ZERO, end - start, thickness, 4, 3);
	line->m_lifeTime = time;
	line->m_colorGradient = colorGradient;
	line->m_isBillboard = false;
	s->m_WorldObjects.push_back(line);
	return line;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawArrow3D(const Vec3& start, const Vec3& end, float thickness, float headSize, float time/*=-1.f*/, const RgbaGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* arrow = new DebugRenderObject(s->m_renderer);
	arrow->m_position = start;
	arrow->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	Vec3 shaftDisp = end - start;
	shaftDisp.setLength(shaftDisp.GetLength() - headSize);
	arrow->m_cpuMesh->AddCylinderToMesh(Vec3::ZERO, shaftDisp, thickness, 4, 3);
	arrow->m_cpuMesh->AddConeToMesh(shaftDisp, headSize * 0.4f, end - start);
	arrow->m_lifeTime = time;
	arrow->m_colorGradient = colorGradient;
	arrow->m_isBillboard = false;
	s->m_WorldObjects.push_back(arrow);
	return arrow;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawBillboardQuad(const Vec3& center, const AABB2& quadShape, TextureView2D* texture/*=nullptr*/, float time/*=-1.f*/, const RgbaGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* quad = new DebugRenderObject(s->m_renderer);
	quad->m_position = center;
	quad->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	quad->m_cpuMesh->AddAABB2ToMesh(quadShape);
	quad->m_lifeTime = time;
	quad->m_colorGradient = colorGradient;
	quad->m_texture = texture;
	quad->m_isBillboard = true;
	s->m_WorldObjects.push_back(quad);
	return quad;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawBillboardText(const Vec3& center, const AABB2& textBoxShape, const BitmapFont* font, float cellHeight, const std::string& text, float time/*=-1.f*/, const Vec2& alignment, const RgbaGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	std::vector<Vertex_PCU> verts;
	font->AddTextInBox(verts, text, textBoxShape, alignment, cellHeight, colorGradient.GetColorAt(0.f));
	DebugRenderObject* btext = new DebugRenderObject(s->m_renderer);
	CPUMesh* cpuMesh = btext->m_cpuMesh;
	for (auto& each : verts) {
		cpuMesh->SetBrushColor(each.m_color);
		cpuMesh->SetBrushUV(each.m_uvTexCoords);
		cpuMesh->AddVertexAndIndex(each.m_position);
	}
	btext->m_position = center;
	btext->m_lifeTime = time;
	btext->m_colorGradient = colorGradient;
	btext->m_texture = font->GetFontTexture();
	btext->m_isBillboard = true;
	btext->m_isTransparent = true;
	s->m_WorldObjects.push_back(btext);
	return btext;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawWireBall(const Vec3& center, float radius, float time/*=-1.f*/, const RgbaGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* ball = new DebugRenderObject(s->m_renderer);
	ball->m_position = center;
	ball->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	ball->m_cpuMesh->AddUVSphereToMesh(center, radius, 8, 8);
	ball->m_lifeTime = time;
	ball->m_colorGradient = colorGradient;
	ball->m_isBillboard = false;
	ball->m_rasterizeMode = DEBUG_RENDER_WIREFRAME;
	ball->m_renderMode = DEBUG_RENDER_DEPTH;
	s->m_WorldObjects.push_back(ball);
	return ball;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawPoint2D(const Vec2& position, float size, float time /*= -1.f*/, const RgbaGradient colorGradient /*= RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* point = new DebugRenderObject(s->m_renderer);
	point->m_position = Vec3(position, 0.f);
	point->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	point->m_cpuMesh->AddAABB2ToMesh(AABB2::MakeAABB2OfSize(size, size) + position);
	point->m_lifeTime = time;
	point->m_colorGradient = colorGradient;
	s->m_screenObjects.push_back(point);
	return point;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawLine2D(const Vec2& start, const Vec2& end, float thickness, float time /*= -1.f*/, const RgbaGradient colorGradient /*= RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* line = new DebugRenderObject(s->m_renderer);
	CPUMesh* cpuMesh = line->m_cpuMesh;
	std::vector<Vertex_PCU> verts;
	AddVerticesOfLine2D(verts, start, end, thickness, colorGradient.GetColorAt(0.f));
	for (auto& each : verts) {
		cpuMesh->SetBrushColor(each.m_color);
		cpuMesh->SetBrushUV(each.m_uvTexCoords);
		cpuMesh->AddVertexAndIndex(each.m_position);
	}
	line->m_lifeTime = time;
	line->m_colorGradient = colorGradient;
	s->m_screenObjects.push_back(line);
	return line;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawQuad2D(const AABB2& quad, TextureView2D* texture /*= nullptr*/, float time /*= -1.f*/, const RgbaGradient colorGradient /*= RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* box = new DebugRenderObject(s->m_renderer);
	box->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	box->m_cpuMesh->AddAABB2ToMesh(quad);
	box->m_lifeTime = time;
	box->m_colorGradient = colorGradient;
	box->m_texture = texture;
	s->m_screenObjects.push_back(box);
	return box;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawText2D(const AABB2& textBox, const BitmapFont* font, float cellHeight, const std::string& text, float time /*= -1.f*/, const Vec2& alignment /*= BitmapFont::ALIGNMENT_CENTER*/, const RgbaGradient colorGradient /*= RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	std::vector<Vertex_PCU> verts;
	font->AddTextInBox(verts, text, textBox, alignment, cellHeight, colorGradient.GetColorAt(0.f));
	DebugRenderObject* btext = new DebugRenderObject(s->m_renderer);
	CPUMesh* cpuMesh = btext->m_cpuMesh;
	for (auto& each : verts) {
		cpuMesh->SetBrushColor(each.m_color);
		cpuMesh->SetBrushUV(each.m_uvTexCoords);
		cpuMesh->AddVertexAndIndex(each.m_position);
	}
	btext->m_lifeTime = time;
	btext->m_colorGradient = colorGradient;
	btext->m_texture = font->GetFontTexture();
	s->m_screenObjects.push_back(btext);
	return btext;
}

////////////////////////////////
DebugRenderer::DebugRenderer(RenderContext* renderer)
	:m_renderer(renderer)
{
	// create default depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;  // for simplicity, just set to true (could set to false if write is false and comprae is always)
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = GetD3DComparisonFunc(COMPARE_GREATEREQ);
	// Stencil - just use defaults for now; 
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	D3D11_DEPTH_STENCILOP_DESC opDesc;
	memset(&opDesc, 0, sizeof(opDesc));
	opDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;      // what to do if stencil fails
	opDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // What to do if stencil succeeds but depth fails
	opDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;      // what to do if the stencil succeeds
	opDesc.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;      // function to test against
	// can have different rules setup for front and backface
	depthStencilDesc.FrontFace = opDesc;
	depthStencilDesc.BackFace = opDesc;
	renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_dsUseDepth);
	
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_dsTransparent);
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthFunc = GetD3DComparisonFunc(COMPARE_ALWAYS);
	renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_dsXray);
	depthStencilDesc.StencilEnable = FALSE;
	renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_dsAlways);

	
	

	// create default rastrization state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	memset(&rasterizerDesc, 0, sizeof(rasterizerDesc));
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;
	renderer->GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_rsNormal);

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	renderer->GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_rsWireframe);

	m_shader = Shader::CreateShaderFromXml("Data/Shaders/unlit.shader.xml", g_theRenderer);

	IntVec2 res = renderer->GetResolution();
	m_screenCamera = new Camera(Vec2(0, 0), Vec2((float)res.x, (float)res.y));
}

////////////////////////////////
DebugRenderer::~DebugRenderer()
{
	DX_SAFE_RELEASE(m_rsWireframe);
	DX_SAFE_RELEASE(m_rsNormal);
	DX_SAFE_RELEASE(m_dsTransparent);
	DX_SAFE_RELEASE(m_dsXray);
	DX_SAFE_RELEASE(m_dsUseDepth);
	DX_SAFE_RELEASE(m_dsAlways);
	delete m_screenCamera;
}