#include "jsonUtility.h"
#include "json.h"
#include <fstream>
#include <stdexcept>



namespace emberAssetLoader
{
	namespace JsonUtility
	{
		nlohmann::json LoadObject(const std::filesystem::path& path)
		{
			// Error handling:
			if (!std::filesystem::exists(path))
				throw std::runtime_error("JsonUtility::LoadObject(...) failed for '" + path.string() + "'. File does not exist.");
			if (!std::filesystem::is_regular_file(path))
				throw std::runtime_error("JsonUtility::LoadObject(...) failed for '" + path.string() + "'. Path is not a regular file.");

			// File loading:
			std::ifstream file(path);
			if (!file.is_open())
				throw std::runtime_error("JsonUtility::LoadObject(...) failed for '" + path.string() + "'. Could not open file.");

			// Json loading:
			nlohmann::json json;
			try
			{
				file >> json;
			}
			catch (const nlohmann::json::exception& exception)
			{
				throw std::runtime_error("JsonUtility::LoadObject(...) failed for '" + path.string() + "'. Invalid JSON: " + std::string(exception.what()));
			}
			if (!json.is_object())
				throw std::runtime_error("JsonUtility::LoadObject(...) failed for '" + path.string() + "'. Root value must be an object.");
			return json;
		}
		bool GetRequiredBool(const nlohmann::json& json, const std::filesystem::path& jsonPath, std::string_view memberName)
		{
			// Json must contain 'memberName':
			std::string memberNameString(memberName);
			if (!json.contains(memberNameString))
				throw std::runtime_error("JsonUtility::GetRequiredBool(...) failed for '" + jsonPath.string() + "'. Missing '" + memberNameString + "'.");

			// memberName value must be a bool:
			const nlohmann::json& value = json.at(memberNameString);
			if (!value.is_boolean())
				throw std::runtime_error("JsonUtility::GetRequiredBool(...) failed for '" + jsonPath.string() + "'. Member '" + memberNameString + "' must be a boolean.");
			return value.get<bool>();
		}
		std::string GetRequiredString(const nlohmann::json& json, const std::filesystem::path& jsonPath, std::string_view memberName)
		{
			// Json must contain 'memberName':
			std::string memberNameString(memberName);
			if (!json.contains(memberNameString))
				throw std::runtime_error("JsonUtility::GetRequiredString(...) failed for '" + jsonPath.string() + "'. Missing '" + memberNameString + "'.");

			// memberName value must be a string:
			const nlohmann::json& value = json.at(memberNameString);
			if (!value.is_string())
				throw std::runtime_error("JsonUtility::GetRequiredString(...) failed for '" + jsonPath.string() + "'. Member '" + memberNameString + "' must be a string.");

			// memberName value string must not be empty:
			std::string stringValue = value.get<std::string>();
			if (stringValue.empty())
				throw std::runtime_error("JsonUtility::GetRequiredString(...) failed for '" + jsonPath.string() + "'. Member '" + memberNameString + "' cannot be empty.");
			return stringValue;
		}
		emberCommon::ResourceAccessRights GetResourceAccessRights(const nlohmann::json& json, const std::filesystem::path& jsonPath)
		{
			// Json must contain accessRights field:
			if (!json.contains("accessRights"))
				throw std::runtime_error("JsonUtility::GetResourceAccessRights(...) failed for '" + jsonPath.string() + "'. Missing 'accessRights'.");

			// accessRights field must be a an object:
			const nlohmann::json& accessRightsJson = json.at("accessRights");
			if (!accessRightsJson.is_object())
				throw std::runtime_error("JsonUtility::GetResourceAccessRights(...) failed for '" + jsonPath.string() + "'. Member 'accessRights' must be an object.");
			for (auto iterator = accessRightsJson.begin(); iterator != accessRightsJson.end(); iterator++)
			{
				const std::string& memberName = iterator.key();
				if (memberName != "isAccessible" && memberName != "isDeletable" && memberName != "isMutable")
					throw std::runtime_error("JsonUtility::GetResourceAccessRights(...) failed for '" + jsonPath.string() + "'. Unknown accessRights member '" + memberName + "'.");
			}

			// Construct resource access rights:
			return emberCommon::ResourceAccessRights
			{
				GetRequiredBool(accessRightsJson, jsonPath, "isAccessible"),
				GetRequiredBool(accessRightsJson, jsonPath, "isDeletable"),
				GetRequiredBool(accessRightsJson, jsonPath, "isMutable")
			};
		}
		std::filesystem::path ResolveFilePath(const std::filesystem::path& jsonPath, const std::string& filePath, std::string_view memberName)
		{
			std::filesystem::path resolvedPath = filePath;
			if (resolvedPath.is_relative())
				resolvedPath = jsonPath.parent_path() / resolvedPath;
			resolvedPath = std::filesystem::absolute(resolvedPath).lexically_normal().make_preferred();
			if (!std::filesystem::exists(resolvedPath))
				throw std::runtime_error("JsonUtility::ResolveFilePath(...) failed for '" + jsonPath.string() + "'. File from '" + std::string(memberName) + "' does not exist: " + resolvedPath.string());
			if (!std::filesystem::is_regular_file(resolvedPath))
				throw std::runtime_error("JsonUtility::ResolveFilePath(...) failed for '" + jsonPath.string() + "'. Path from '" + std::string(memberName) + "' is not a regular file: " + resolvedPath.string());
			return resolvedPath;
		}
	}
}