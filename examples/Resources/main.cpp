#include <BENG/Resources.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

// Example custom async loader for heavy resources
struct SlowTextureLoader : public bg::AsyncResourceLoader<sf::Texture> {
    virtual void load(bg::TextureResourceManager& manager, const std::string& uri,
                      float& progress) override {
        // Pretend this is a heavy resource and takes time to load
        for (int i = 0; i <= 1000; ++i) {
            progress = static_cast<float>(i) / 1000.f;
            sf::sleep(sf::milliseconds(3));
        }

        // The resource must be available in the manager when we complete
        manager.load(uri);
    }
};

// Provide a callback for BackgroundResourceLoader to report progress to
// This can be any method, including a method in a class with plenty of state information
void progressCallback(float progress) {
    std::cout << "Loading: " << (progress * 100.f) << "%\n";
}

int main() {
    // Create the resource manager for textures
    bg::TextureResourceLoader textureLoader;
    bg::TextureResourceManager textureManager(textureLoader);

    // Load a managed texture using the new resource manager
    bg::Resource<sf::Texture> texture = textureManager.load("image.png");

    // Pretend we are loading a heavy resource(s)
    // Background loaders should be created on the fly before loading screens
    SlowTextureLoader slowLoader;
    bg::BackgroundResourceLoader<sf::Texture> backgroundLoader(
        textureManager, slowLoader, &progressCallback, 0.01f);

    // Add each uri we want to load in the background
    backgroundLoader.addResourceToQueue("image.png", 1);
    // This one is longer to load (pretending)
    backgroundLoader.addResourceToQueue("image.png", 5);

    // Do the load. This will block until loading is complete
    // A backgorund thread will call our monitoring function, which can update loading screen
    backgroundLoader.load();

    return 0;
}