#include "Renderer3D.h"
#include "RenderCommand.h"

namespace Quark {

	void Renderer3D::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		//s_Data.DefaultTexture->Attach();

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}

	void Renderer3D::Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		texture->Attach();

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}

	void Renderer3D::Submit(const Ref<Shader>& shader, const Ref<Framebuffer>& framebuffer, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		framebuffer->AttachColorAttachment(0);

		shader->Attach();
		shader->SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(va);
	}
}
