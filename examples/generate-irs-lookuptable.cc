/*
 * Copyright (c) 2024 Jakob Rühlow
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/irs-model.h"
#include "ns3/irs-spectrum-model.h"

#include <cstdint>
#include <fstream>
#include <mutex>
#include <sstream>
#include <sys/types.h>
#include <thread>
#include <vector>

using namespace ns3;

class IrsWorker
{
  private:
    static std::mutex file_mutex;
    std::ofstream& output_file;
    Ptr<IrsSpectrumModel> irs;
    double lambda;
    int start_i;
    int end_i;

  public:
    IrsWorker(std::ofstream& file,
              Ptr<IrsSpectrumModel> irs_ptr,
              double lambda_val,
              int start,
              int end)
        : output_file(file),
          irs(irs_ptr),
          lambda(lambda_val),
          start_i(start),
          end_i(end)
    {
    }

    void operator()()
    {
        std::stringstream buffer;

        for (int i = start_i; i < end_i; ++i)
        {
            for (int j = 0; j < 181; ++j)
            {
                IrsEntry newEntry =
                    irs->GetIrsEntry(Angles(DegreesToRadians(i), DegreesToRadians(0)),
                                     Angles(DegreesToRadians(j), DegreesToRadians(0)),
                                     lambda);

                buffer << i << "," << j << "," << newEntry.gain << "," << newEntry.phase_shift
                       << "\n";
            }
        }

        std::lock_guard<std::mutex> lock(file_mutex);
        output_file << buffer.str();
    }
};

std::mutex IrsWorker::file_mutex;

void
ProcessIrsEntriesParallel(Ptr<IrsSpectrumModel> irs, double lambda, const std::string& filename)
{
    std::ofstream output_file(filename);
    output_file << "i,j,gain,phase_shift\n";

    const int num_threads =
        std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4;

    const int total_rows = 181;
    const int rows_per_thread = total_rows / num_threads;

    std::vector<std::thread> threads;

    for (int t = 0; t < num_threads; ++t)
    {
        int start = 0 + (t * rows_per_thread);
        int end = (t == num_threads - 1) ? 181 : start + rows_per_thread;

        threads.emplace_back(IrsWorker(std::ref(output_file), irs, lambda, start, end));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    output_file.close();
}

int
main(int argc, char* argv[])
{
    uint16_t Nr = 20;
    uint16_t Nc = 20;
    double dr = 0.029106063883495;
    double dc = 0.029106063883495;
    double freq = 5.15e9;
    double dApSta = 40;
    double dApIrsSta = 40.2962;
    double delta = 0;
    uint16_t inAngle = 135;
    uint16_t outAngle = 89;
    uint16_t samples = 1000;
    std::string output_file = "irs_entries.csv";

    CommandLine cmd(__FILE__);
    cmd.AddValue("Nr", "Number of rows", Nr);
    cmd.AddValue("Nc", "Number of columns", Nc);
    cmd.AddValue("dr", "Row spacing", dr);
    cmd.AddValue("dc", "Column spacing", dc);
    cmd.AddValue("freq", "Frequency", freq);
    cmd.AddValue("dApSta", "AP-STA distance", dApSta);
    cmd.AddValue("dApIrsSta", "AP-IRS-STA distance", dApIrsSta);
    cmd.AddValue("delta", "Delta value", delta);
    cmd.AddValue("inAngle", "Input angle", inAngle);
    cmd.AddValue("outAngle", "Output angle", outAngle);
    cmd.AddValue("samples", "Number of samples", samples);
    cmd.AddValue("output", "Output CSV file", output_file);
    cmd.Parse(argc, argv);

    double lambda = 299792458.0 / freq;

    Ptr<IrsSpectrumModel> irs = CreateObject<IrsSpectrumModel>();
    irs->SetDirection(Vector(0, 1, 0));
    irs->SetSamples(samples);
    irs->SetN({Nr, Nc});
    irs->SetSpacing({dr, dc});
    irs->SetFrequency(freq);
    irs->CalcRCoeffs(dApSta,
                     dApIrsSta,
                     Angles(DegreesToRadians(inAngle), 0),
                     Angles(DegreesToRadians(outAngle), 0),
                     delta);

    ProcessIrsEntriesParallel(irs, lambda, output_file);

    return 0;
}
