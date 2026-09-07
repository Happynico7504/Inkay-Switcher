#pragma once

#include <atomic>
#include <string>

enum DownloadState {
    DOWNLOADSTATE_SELECT,
    DOWNLOADSTATE_HBAS_WARNING,
    DOWNLOADSTATE_SELECT_ENV,
    DOWNLOADSTATE_DOWNLOADING,
    DOWNLOADSTATE_ERROR,
    DOWNLOADSTATE_FINISHED
};

namespace Inkay {
    namespace Download {
        extern std::string LastError;
        extern std::atomic<bool> HasError;
        extern int PendingIndex; // index into Inkay::Repos::Sources, -1 = none selected
        extern std::string SelectedEnvironment;
        extern std::atomic<DownloadState> State;

        void StartSourceDownload(int index);
        void UpdateDownloads(void);
    }
}
