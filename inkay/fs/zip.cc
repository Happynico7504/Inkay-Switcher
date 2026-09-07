#include "eg2/libs/typedefs.h"
#include "eg2/libs/miniz.hh"
#include "eg2/sys/report.hh"
#include "inkay/fs/files.hh"
#include <vector>

namespace Inkay {
    namespace ZIP {
        bool Extract(const std::vector<u8>& zipData) {
            mz_zip_archive zip;
            memset(&zip, 0, sizeof(zip));

            if (!mz_zip_reader_init_mem(&zip, zipData.data(), zipData.size(), 0)) {
                SYS::Report::Log("Inkay::ZIP::Extract(): ZIP init failed\n");
                return false;
            }

            int count = (int)mz_zip_reader_get_num_files(&zip);
            bool wroteWMS = false;
            bool wroteWPS = false;

            for (int i = 0; i < count; i++) {
                mz_zip_archive_file_stat st;

                if (!mz_zip_reader_file_stat(&zip, i, &st)) continue;

                std::string name = st.m_filename;

                usize size = 0;
                void* data = mz_zip_reader_extract_to_heap(&zip, i, &size, 0);

                if (!data || !size) continue;

                if (name.find(".wms") != std::string::npos) {
                    wroteWMS = Inkay::Files::WriteWMS(data, size);
                    SYS::Report::Log("Inkay::ZIP::Extract(): WMS extracted (%s)\n", wroteWMS ? "ok" : "write failed");
                }

                if (name.find(".wps") != std::string::npos) {
                    wroteWPS = Inkay::Files::WriteWPS(data, size);
                    SYS::Report::Log("Inkay::ZIP::Extract(): WPS extracted (%s)\n", wroteWPS ? "ok" : "write failed");
                }

                free(data);
            }

            mz_zip_reader_end(&zip);

            if (!wroteWMS || !wroteWPS) {
                SYS::Report::Log("Inkay::ZIP::Extract(): archive missing expected files or write failed (WMS: %d, WPS: %d)\n", wroteWMS, wroteWPS);
                return false;
            }

            return true;
        }
    }
}