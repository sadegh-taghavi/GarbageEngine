#pragma once

#define GEW_SETTINGS_FILE "geditor.ini"
#ifdef _DEBUG
	#define GEW_SHADER_PATH "../../GEditorData/Shader/Debug/"
	#define GEW_TEXTURE_PATH "../../GEditorData/Texture/"
	#define GEW_MODEL_PATH	"../../GEditorData/Model/"
#else
	#define GEW_SHADER_PATH "../../GEditorData/Shader/Release/"
	#define GEW_TEXTURE_PATH "../../GEditorData/Texture/"
	#define GEW_MODEL_PATH	"../../GEditorData/Model/"
#endif