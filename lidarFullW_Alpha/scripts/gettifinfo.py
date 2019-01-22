#File name: gettifinfo.py
#Created on: 17-December-2018
#Author: Aaron Orenstein

from osgeo import gdal
import sys
import struct
import os
gdal.UseExceptions();

input_files = []
file_name = ""
custom_path = ""

def main(band_num, input_file):
  #Make sure input file is .tif
  if input_file == "-i":
    Finish(0)
  last_slash = input_file.rfind("/") + 1;
  global file_name
  file_name = input_file[last_slash:]
  print ("\n\33[32mProcessing:\33[0m {}\n".format(file_name))
  if not input_file.endswith(".tif"):
    print ("Invalid file\nFile types supported: .tif")
    Finish(1)        

  #Obtain raster
  raster = gdal.Open( input_file )
  if raster is None:
    print ('Unable to open %s' % input_file);
    Finish(1)

  try:
    band = raster.GetRasterBand(band_num)
  except (RuntimeError, e):
    print ('No band %i found' % band_num);
    print (e);
    Finish(1)

  if band.GetNoDataValue() == None:
    band.SetNoDataValue(-99999)
  if "-i" in sys.argv:
    print ("[ NO DATA VALUE ] = ", band.GetNoDataValue());
    print ("[ MIN ] = ", band.GetMinimum());
    print ("[ MAX ] = ", band.GetMaximum());
    print ("[ SCALE ] = ", band.GetScale());
    print ("[ UNIT TYPE ] = ", band.GetUnitType());
  ctable = band.GetColorTable()

  if ctable is None:
    print ('No ColorTable found');
  else:
    print ("[ COLOR TABLE COUNT ] = ", ctable.GetCount());
    for i in range( 0, ctable.GetCount() ):
      entry = ctable.GetColorEntry( i )
      if not entry:
        continue;
      print ("[ COLOR ENTRY RGB ] = ", ctable.GetColorEntryAsRGB( i, entry ));

  Data(band);

def Data(band):
  raster_array = band.ReadAsArray();
  no_value = band.GetNoDataValue();
  #Go through each piece of the band
  #Print data to file
  print ("\nWriting data to file")
  global file_name
  global custom_path
  output = open(custom_path + file_name[:-4] + ".out", 'w')
  output.write("Max Y = {}\n\n".format(band.YSize - 1))
  for i in range(band.YSize):
    scanline = band.ReadRaster(xoff=0, yoff=i,
                               xsize=band.XSize, ysize=1,
                               buf_xsize=band.XSize, buf_ysize=1,
                               buf_type=gdal.GDT_Float32)
    tuple_of_floats = struct.unpack('f' * band.XSize, scanline)
    if len(tuple_of_floats) != 0:
      output.write("y = {}: ".format(i))
    #Print out data
    for idx, val in enumerate(tuple_of_floats):
      last = idx == len(tuple_of_floats) - 1
      #Get data
      if not (val > band.GetNoDataValue() - 1 and val < band.GetNoDataValue() + 1):
        output.write(str(val) + ("" if last else ", "))
      else:
        output.write("NA" + ("" if last else ", "))
    output.write("\n\n") 
  output.close()
  Finish(0);

def Finish(err):
  input_files.pop(0)
  #Check for extra input files
  if len(input_files) > 0:
    main(1, input_files[0])
  else:
    print ("\nNo more files\n")
    sys.exit(err)

if __name__ == '__main__':
  usage = """
    Usage:
    $ python {} input-raster1 [input-raster2 ...] [i] [-p path_name]
        -i: prints extra information about the tif file
    	-p path_name: Output file destination from your current location
    """.format(sys.argv[0])

  #Check arguments
  i = 1
  while i < len(sys.argv):
    if sys.argv[i] == "-p":
      #Make sure next argument is a valid path
      i += 1
      if len(sys.argv) == i or not os.path.isdir(sys.argv[i]):
        print ("""
    [ Error ] invalid or missing path

    {}""".format(usage))
        sys.exit(1)
      else:
        custom_path = sys.argv[i]
    elif sys.argv[i] != '-i':
      #Store any input files
      input_files.append(sys.argv[i])
    i += 1
  if len(input_files) == 0:
    print ("""
    [ Error ] you must supply at least 1 argument:
    1) input raster

    {}""".format(usage))
    sys.exit(1)

main(1, sys.argv[1]);
