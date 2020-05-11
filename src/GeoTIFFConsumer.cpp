#include <list>
#include <string>
#include <vector>

#include <algorithm>

#include <gdal.h>

#include "spdlog/spdlog.h"

#include "GeoTIFFConsumer.hpp"
#include "GDALWriter.hpp"
#include "PeakProducts.hpp"

GeoTIFFConsumer::GeoTIFFConsumer(double minX, double maxX, double minY, double maxY, std::string fileNamePrefix, std::string coordSys, int utm, std::vector<PeakProducts::Product> products):
        volume_(minX, maxX, minY, maxY), fileNamePrefix_(fileNamePrefix), coordSys_(coordSys), utm_(utm), products_(products)
{}

void GeoTIFFConsumer::postProcess(){
    //Product a file for each product
    GDALAllRegister();
    GDALDriver* tiffDriver = GetGDALDriverManager()->GetDriverByName("GTiff");

    if(!tiffDriver){
        spdlog::critical("[GeoTIFFConsumer] Failed to create GDAL GTiff driver");
        return;
    }

    std::vector<float> columnBuffer;
    const int bufferSize = volume_.ySize;
    columnBuffer.reserve(bufferSize);

    for(const PeakProducts::Product& product : products_){
        std::string fileName = fileNamePrefix_ + "_" + PeakProducts::productString(product) + ".tif";
        std::string fileDesc = "desc";  //@@TODO get from somewhere

        GDALWriter data(*tiffDriver, fileName, fileDesc, volume_.xSize, volume_.ySize);
        data.orient(coordSys_, utm_, volume_.xMin, volume_.yMax);

        int maxPeaks = 0;
        int avgPeaks = 0;
        int numWaves = 0;

        //Loop through all peaks
        for(int x = 0; x < volume_.xSize; ++x){
            //Clear previous values, and fill with no data value
            columnBuffer.clear();
            columnBuffer.resize(bufferSize, GDALWriter::GDAL_NO_DATA);

            for(int y = 0; y < volume_.ySize; ++y){
                const std::list<Peak>* peaks = volume_.getPeaks(x, y);
                if(peaks){  //Only update the buffer if we have peaks there
                    maxPeaks = std::max(maxPeaks, static_cast<int>(peaks->size()));
                    avgPeaks += peaks->size();
                    numWaves++;
                    columnBuffer[volume_.ySize-1-y] = PeakProducts::produceProduct(*peaks, product);    //The volume_.ySize-1 is because it is inverted otherwise
                }
            }

            bool success = data.writeColumn(columnBuffer, x);
            if(!success){
                spdlog::error("[GeoTIFFConsumer] Failed to write column with offset of {} to file", x);
            }
        }
    }
}

void GeoTIFFConsumer::consumePeaks(const std::vector<Peak>& peaks, const PulseData&){
    for(const Peak& peak : peaks){
        volume_.insertPeak(peak);   //@@TODO: What to do if peak rejected?
    }
}