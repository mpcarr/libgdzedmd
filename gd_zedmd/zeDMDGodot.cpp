#include "ZeDMDGodot.h"
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/sub_viewport_container.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

ZeDMDGodot::ZeDMDGodot()
{
    // if (!Engine::get_singleton()->is_editor_hint())
    //{
    zedmd_instance = new ZeDMD(); // Create ZeDMD instance
    //}
}

ZeDMDGodot::~ZeDMDGodot()
{
    if (zedmd_instance)
    {
        close_connection();
        // delete zedmd_instance; // Clean up instance
        UtilityFunctions::print("ZeDMDGodot Connection Closed");
    }
}

void ZeDMDGodot::_ready()
{
    UtilityFunctions::print("ZeDMDGodot is ready!");

    if (!Engine::get_singleton()->is_editor_hint())
    {
        open_connection();
        if (m_zeDMD_debug)
        {
            zedmd_instance->EnableDebug();
        }
        device_width = zedmd_instance->GetWidth();
        device_height = zedmd_instance->GetHeight();
        UtilityFunctions::print("zeDMD: " + String::num_int64(device_width) + "x" + String::num_int64(device_height));
        zedmd_instance->SetFrameSize(device_width, device_height);

        uint8_t init_palette[64 * 3] = {
            20, 14, 30, 45, 26, 113, 50, 87, 190, 64, 157, 239, 112, 219, 255, 191, 255, 255,
            62, 50, 213, 110, 106, 255, 166, 173, 255, 216, 224, 255, 101, 43, 188, 180, 76, 239,
            236, 140, 255, 255, 205, 255, 72, 14, 85, 148, 24, 135, 228, 68, 195, 255, 145, 226,
            25, 12, 18, 85, 14, 43, 175, 16, 46, 255, 66, 79, 255, 151, 146, 255, 213, 207,
            73, 29, 30, 170, 44, 30, 246, 109, 30, 255, 174, 104, 255, 225, 181, 73, 41, 23,
            151, 83, 15, 221, 140, 0, 251, 200, 0, 255, 246, 153, 12, 16, 27, 14, 62, 18,
            56, 116, 26, 108, 179, 40, 175, 227, 86, 228, 252, 162, 13, 56, 76, 23, 117, 120,
            0, 188, 159, 107, 236, 189, 201, 252, 204, 53, 50, 52, 102, 93, 91, 153, 141, 134,
            205, 191, 179, 234, 230, 218, 47, 49, 67, 80, 93, 109, 123, 149, 160, 166, 207, 208,
            223, 234, 228, 141, 65, 49, 203, 115, 77, 239, 175, 121, 156, 43, 59, 228, 87, 97,
            255, 255, 255, 0, 0, 0, 228, 22, 43, 255, 255, 64};

        // Copy the initial palette to the instance's palette
        std::copy(std::begin(init_palette), std::end(init_palette), std::begin(palette));
        zedmd_instance->SetPalette(palette, 64);
        zedmd_instance->ClearScreen();
        //zedmd_instance->LedTest();
        zedmd_instance->DisableRGB24Streaming();
        zedmd_instance->DisableUpscaling();
        size_t total_pixels = device_width * device_height;
        std::vector<uint8_t> frame(total_pixels);

        for (int y = 0; y < device_height; ++y)
        {
            for (int x = 0; x < device_width; ++x)
            {
                uint8_t r = static_cast<uint8_t>(20);
                uint8_t g = static_cast<uint8_t>(14);
                uint8_t b = static_cast<uint8_t>(30);

                // Use the combined approach to find the palette index
                frame[y * device_width + x] = find_palette_index(r, g, b, palette, 64);
            }
        }

        // Call RenderColoredGray6 with the indexed frame
        zedmd_instance->RenderColoredGray6(frame.data(), nullptr);
    }
}

void ZeDMDGodot::_process(double delta)
{
    if (!Engine::get_singleton()->is_editor_hint())
    {
        //render_viewport();
    }
}

uint8_t ZeDMDGodot::find_palette_index(uint8_t r, uint8_t g, uint8_t b, const uint8_t *palette, size_t palette_size)
{
    // Direct matching
    for (size_t i = 0; i < palette_size; ++i)
    {
        if (palette[i * 3] == r && palette[i * 3 + 1] == g && palette[i * 3 + 2] == b)
        {
            return static_cast<uint8_t>(i); // Return the exact match index
        }
    }

    // Fallback to nearest neighbor
    uint8_t closest_index = 0;
    int min_distance = INT_MAX;

    for (size_t i = 0; i < palette_size; ++i)
    {
        int pr = palette[i * 3];     // Palette red
        int pg = palette[i * 3 + 1]; // Palette green
        int pb = palette[i * 3 + 2]; // Palette blue

        // Calculate the squared Euclidean distance
        int distance = (pr - r) * (pr - r) +
                       (pg - g) * (pg - g) +
                       (pb - b) * (pb - b);

        // Update if a closer color is found
        if (distance < min_distance)
        {
            min_distance = distance;
            closest_index = static_cast<uint8_t>(i);
        }
    }

    return closest_index; // Return the nearest neighbor index
}

bool ZeDMDGodot::open_connection()
{
    if (zedmd_instance)
    {
        //zedmd_instance->SetWiFiSSID("ZeDMD-Wifi");
        //zedmd_instance->SetWiFiPassword("zedmd1234");
        //zedmd_instance->SetWiFiPort(3333);
        return zedmd_instance->OpenDefaultWiFi();
    }
    return false;
}

void ZeDMDGodot::close_connection()
{
    if (zedmd_instance)
    {
        UtilityFunctions::print("Closing Connection");
        zedmd_instance->Close();
    }
}

void ZeDMDGodot::render_viewport()
{
    if (zedmd_instance)
    {
        //
        // Get the viewport associated with this control
        Viewport *viewport = get_viewport();

        if (!viewport)
        {
            UtilityFunctions::print("Failed to retrieve viewport.");
            return;
        }

        // Get the viewport texture
        Ref<ViewportTexture> viewport_texture = viewport->get_texture();
        if (viewport_texture.is_null())
        {
            UtilityFunctions::print("Failed to retrieve viewport texture.");
            return;
        }

        // Get the image from the viewport texture
        Ref<Image> image = viewport_texture->get_image();
        if (image.is_null())
        {
            UtilityFunctions::print("Failed to retrieve image data.");
            return;
        }
       
        // Create an indexed frame buffer
        size_t total_pixels = device_width * device_height;
        std::vector<uint8_t> frame(total_pixels);

        for (int y = 0; y < device_height; ++y)
        {
            for (int x = 0; x < device_width; ++x)
            {
                Color color = image->get_pixel(x, y);
                uint8_t r = static_cast<uint8_t>(color.r * 255);
                uint8_t g = static_cast<uint8_t>(color.g * 255);
                uint8_t b = static_cast<uint8_t>(color.b * 255);

                // Use the combined approach to find the palette index
                frame[y * device_width + x] = find_palette_index(r, g, b, palette, 64);
            }
        }

        // Call RenderColoredGray6 with the indexed frame
        zedmd_instance->RenderColoredGray6(frame.data(), nullptr);
    }
}

void ZeDMDGodot::set_zeDMDDebug(const bool p_debug)
{
    m_zeDMD_debug = p_debug;
}

bool ZeDMDGodot::get_zeDMDDebug() const
{
    return m_zeDMD_debug;
}

void ZeDMDGodot::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_zeDMDDebug"), &ZeDMDGodot::get_zeDMDDebug);
    ClassDB::bind_method(D_METHOD("set_zeDMDDebug", "p_debug"), &ZeDMDGodot::set_zeDMDDebug);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "Debug"), "set_zeDMDDebug", "get_zeDMDDebug");
}