#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <libzedmd/src/ZeDMD.h>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/control.hpp>

using namespace godot;

class ZeDMDGodot : public Control {
    GDCLASS(ZeDMDGodot, Control);

private:
    ZeDMD *zedmd_instance = nullptr;
    uint16_t device_width;
    uint16_t device_height;
    uint8_t palette[64*3];

    bool m_zeDMD_debug = false;

public:
    ZeDMDGodot();
    ~ZeDMDGodot();

    void _process(double delta) override;
    virtual void _ready();

    bool open_connection();
    void close_connection();
    void render_viewport();
    uint8_t find_palette_index(uint8_t r, uint8_t g, uint8_t b, const uint8_t *palette, size_t palette_size);
    void set_zeDMDDebug(const bool p_debug);
	bool get_zeDMDDebug() const;

protected:
    static void _bind_methods();
};
