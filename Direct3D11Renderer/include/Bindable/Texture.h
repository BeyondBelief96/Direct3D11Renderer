#pragma once

#include "BindableCommon.h"
#include "Utilities/TextureLoader.h"
#include <wrl.h>
#include <string>
#include <memory>

/** @brief Texture bindable resource for D3D11 rendering.
 *
 *  Loads texture images using DirectXTex library and creates
 *  D3D11 shader resource views for use in pixel shaders. Supports automatic
 *  alpha channel detection by examining both pixel format and actual alpha values.
 */
class Texture : public Bindable
{
public:
	/** @brief Constructs a texture from an image file.
	 *  @param gfx Graphics context for D3D11 operations
	 *  @param path File path to the texture image
	 *  @param slot Shader resource slot to bind to (0-127)
	 *  @param alphaLoaded Override for alpha channel detection (optional)
	 *  @note Alpha channel is automatically detected unless overridden
	 */
	Texture(Graphics& gfx, const std::string& path, UINT slot, bool alphaLoaded = false);
	
	/** @brief Binds this texture to the pixel shader.
	 *  @param gfx Graphics context for binding operations
	 */
	void Bind(Graphics& gfx) noexcept override;
	
	/** @brief Gets the unique identifier for this texture.
	 *  @return UID string combining type name, path, and slot
	 */
	std::string GetUID() const noexcept override;

	/** @brief Resolves a texture from the bindable cache or creates new one.
	 *  @param gfx Graphics context
	 *  @param path File path to the texture
	 *  @param slot Shader slot (defaults to 0)
	 *  @return Shared pointer to cached or newly created texture
	 */
	static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0);
	
	/** @brief Generates a unique identifier string for caching.
	 *  @param path File path of the texture
	 *  @param slot Shader resource slot
	 *  @return Generated UID string
	 */
	static std::string GenerateUID(const std::string& path, UINT slot);
	
	/** @brief Checks if the texture has an active alpha channel.
	 *  @return True if alpha channel contains non-255 values
	 *  @note Detection is performed during texture loading
	 */
	bool AlphaChannelLoaded() const noexcept;
private:
	/** @brief Internal method to load texture from file path.
	 *  @param gfx Graphics context
	 *  @param path File path to texture
	 *  @note Handles texture loading, format conversion, and alpha detection using DirectXTex
	 */
	void LoadFromFile(Graphics& gfx, const std::string& path);
	
	unsigned int slot;                    /**< Shader resource slot index */
	std::string path;                     /**< Original file path */
	bool alphaChannelLoaded = false;      /**< True if alpha channel is actively used */
	std::unique_ptr<ITextureLoader> textureLoader;  /**< Texture loading abstraction */
	
protected:
	/** @brief D3D11 shader resource view for the texture */
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};