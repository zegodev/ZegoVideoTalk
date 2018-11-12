cmake --build . --config Release || exit 1
macdeployqt ./VideoTalk/Release/VideoTalk.app || exit 1