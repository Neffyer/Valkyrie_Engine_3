//#ifndef __RESOURCEANIMATIONLOADER_H__
//#define __RESOURCEANIMATIONLOADER_H__
//
//#include "Globals.h"
//#include <map>
//
//#include "../External/MathGeoLib/include/Math/float3.h"
//#include "../External/MathGeoLib/include/Math/Quat.h"
//
//class ResourceAnimation;
//class ResourceAnimatorController;
//
//struct Channel;
//
//struct aiAnimation;
//struct aiNodeAnim;
//
//
//namespace Importer
//{
//	namespace Animations
//	{
//		ResourceAnimation* Create();
//
//		void Import(const aiAnimation* animation, ResourceAnimation* resourceAnimation);
//		uint64 Save(const ResourceAnimation* resourceAnimation, char** buffer);
//		void Load(const char* buffer, ResourceAnimation* resourceAnimation);
//
//		namespace Private
//		{
//			void ImportChannel(const aiNodeAnim* node, Channel& channel);
//
//			uint CalcChannelSize(const Channel& channel);
//			void SaveChannel(const Channel& channel, char** cursor);
//			void SaveChannelKeys(const std::map<double, float3>&, char** cursor);
//			void SaveChannelKeys(const std::map<double, Quat>&, char** cursor);
//
//			void LoadChannel(Channel& channel, const char** cursor);
//			void LoadChannelKeys(std::map<double, float3>& map, const char** cursor, uint size);
//			void LoadChannelKeys(std::map<double, Quat>& map, const char** cursor, uint size);
//		}
//	}
//
//	namespace Animators
//	{
//		ResourceAnimatorController* Create();
//
//		uint64 Save(const ResourceAnimatorController* resourceAnimation, char** buffer);
//		void Load(const char* buffer, ResourceAnimatorController* resourceAnimation);
//	}
//}
//#endif