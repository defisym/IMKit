#include "FontEx.h"

#include <d3d11.h>
#include <_DeLib/WindowsCommon.h>

#include "macro.h"

size_t FontInfo::GetHash() const {
    return std::hash<std::string>()(name) ^ std::hash<float>()(size);
}

FontEx::FontEx() {
    pFontAtlas = ImGui::GetIO().Fonts;

#ifdef FONT_SIMPLIFIED_CHINESE_COMMON_ONLY
    builder.AddRanges(pFontAtlas->GetGlyphRangesChineseSimplifiedCommon());
#else
    // build CKJ by default
    builder.AddRanges(pFontAtlas->GetGlyphRangesChineseFull());
    // Chinese includes Japaneses
    //builder.AddRanges(pFontAtlas->GetGlyphRangesJapanese());
    builder.AddRanges(pFontAtlas->GetGlyphRangesKorean());
#endif
    builder.BuildRanges(&ranges);

    pFontAtlas->TexDesiredWidth = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;

#ifdef FONT_LOAD_ALL_FONTS
    // load all fonts in font folder
    //  not work: imgui will merge all char into a texture
    //            too many fonts will exceed the limit of D3D
    //            only one font usually is okay here
    std::vector<std::wstring> fileList;
    GetFileList(&fileList, L"./Fonts");

    for(auto& it:fileList) {
        AddFont(to_byte_string(it).c_str(), 18.0f);
        pFontAtlas->Build();
    }
#else
    AddFont(R"(c:\Windows\Fonts\msyh.ttc)", 18.0f);
    //AddFont("./Fonts/SourceHanSansCN-Regular.ttf", 18.0f);
#endif
}

// Load Fonts
// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
// - Read 'docs/FONTS.md' for more instructions and details.
// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
//io.Fonts->AddFontDefault();
//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
//IM_ASSERT(font != nullptr);
bool FontEx::AddFont(const char* filename, float size_pixels) {
    auto pFont = pFontAtlas->AddFontFromFileTTF(filename, size_pixels, &conf, ranges.Data);
    if (pFont == nullptr) { return false; }

    const auto fontInfo = FontInfo{ filename,size_pixels };
    fontLib.emplace(fontInfo.GetHash(), pFont);

    return true;
}

#ifdef _DEBUG
void FontEx::FontDebug() {
    // ------------------------------------------
    // Pass
    // ------------------------------------------
    ImGui::DebugTextEncoding(u8"简体中文");
    ImGui::DebugTextEncoding(u8"高咲侑 桜坂しずく　にじがさき");
    ImGui::DebugTextEncoding(to_byte_string(L"高咲侑 桜坂しずく　にじがさき", CP_UTF8).c_str());

    // ------------------------------------------
    // Invalid
    // ------------------------------------------
    ImGui::DebugTextEncoding(to_byte_string(L"高咲侑 桜坂しずく　にじがさき", CP_ACP).c_str());
}
#endif