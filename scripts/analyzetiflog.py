#File Name: analyzetif.py
#Author: Aaron Orenstein
#Created On: 20 April 2019

import sys
import os
import math
import numpy as np
from PIL import Image, ImageDraw, ImageFont
from tif import Tif

def main():
  print ()
  #Stores the tif file name
  file_names = []
  #Stores the path to ouput files
  custom_path = "./"
  #Stores what outputs to create
  image_output = False
  text_output = False

  #Create the usage message
  usage = """
    Usage:
    $ python {} -f file_name -wi [-p pathname]
       -f filename: Name of tif file to be analyzed
       -w: Creates a .out text file containing the data of the tif file
       -i: Creates a heatmap png of the tif file
       -p pathname: Writes all output files to the given path
          """.format(sys.argv[0])

  #Stores any argument errors
  arg_errors = []

  #Iterate through every argument
  i = 1
  while i < len(sys.argv):
    arg = sys.argv[i]
    #Get length of arg_errors
    l = len(arg_errors)
    #Argument is a file
    if arg == '-f':
      try:
         while True:
          name = sys.argv[i + 1]
          #File does not have '.tif' extension
          if not name.endswith(".tif"):
            if len(file_names) == 0:
              arg_errors.append("Invalid file type, must have extension '.tif'")
            break
          else:
            file_names.append(name)
          i += 1       
      #No file was input
      except IndexError:
        arg_errors.append("No input file provided")
    #Custom path
    elif arg == '-p':
      #Make sure next argument is a valid path
      try:
        custom_path = sys.argv[i + 1]
        if not os.path.isdir(custom_path):
          arg_errors.append("Invalid path")
        else:
          custom_path += "/" if custom_path[-1:] != "/" else ""
        i += 1
      except IndexError:
        arg_errors.append("No path provided")
    #Check for any outputs to create
    elif arg in ['-w','-i','-wi','-iw']:
      text_output = 'w' in arg or text_output
      image_output = 'i' in arg or image_output
    else:
      arg_errors.append("Invalid argument")
    #If any errors were added, include the argument
    if l != len(arg_errors):
      arg_errors[l] = "'{}' ".format(sys.argv[i]) + arg_errors[l]
    i += 1

  #Make sure at least '-i' or '-w' were included
  if not image_output and not text_output:
    arg_errors.append("Please include one or both of '-i' and '-w'")

  done = "\n\33[32m[ Complete ]\33[0m:"
  fail = "\33[31m[ Error ]\33[0m:"

  #Check for argument errors and print them
  if len(arg_errors) != 0:
    for err in arg_errors:
      print (fail, err)
    print(usage)
    sys.exit(1)
 
  #Run the tif file
  for name in file_names:
    tif = Tif(name)
    if text_output:
      writeData(tif, custom_path)
      print (done, "Data written to {}.out\n".format(
             custom_path + tif.file_name_trimmed))
    if image_output:
      createImage(tif, custom_path)
      print (done, "Heatmap has been saved to {}.png\n".format(
             custom_path + tif.file_name_trimmed))

#Write data to .out file
def writeData(tif, path):
  #Go through each data point in the band
  #Print data to file
  print ("Writing data to file", end='')
  #Create output file
  output = open(path + tif.file_name[:-4] + ".out", 'w')
  output.write("Max Y = {}\n\n".format(tif.maxY))

  #Go through each y value
  for y, vals in enumerate(tif.data):
    output.write("y = {}: ".format(y))
    for x, val in enumerate(vals):
      #Check if it's the last x value
      last = x == len(vals) - 1
      #Make sure the data is not the no data value
      if val != tif.no_value:
        #Write data to output file and add it do the data list
        output.write(str(val) + ("" if last else ", "))
      else:
        #Write 'NA' for no data and indicate it in data list
        output.write("NA" + ("" if last else ", "))
    output.write("\n\n")
    print ("\rWriting data to file {}%".format(
             round(y*100/(tif.maxY))), end="")
  output.close()

#Create heatmap image
def createImage(tif, path):
  #Dimension conventions
  #Width (w) is always measured in columnns
  #Height (h) is always measured in rows
  #Each column and row has a width/height of 1 pixel (px)

  print ("Creating heatmap", end="")

  #Get dimensions of data
  data_w, data_h = tif.data.shape[1], tif.data.shape[0]
  #Min/max values of the data array
  data_values = [i for i in tif.data.flatten() if math.isfinite(i) and
                 i != tif.no_value]
  max_val, min_val = max(data_values), min(data_values)
  #Get the string of these values rounded to 2 decimal places
  max_str, min_str = str(round(max_val, 2)), str(round(min_val, 2))

  #Get an array with the log of all our data points
  log_values = [math.log10(i) for i in data_values if i > 0]
  #Calculate the lowest power of ten in log_vals
  #This will also be our offset when converting from powers of ten
  #to list indexes
  lower = math.floor(min(log_values))
  #Get the number of section (unique powers of ten)
  num_sections = math.ceil(max(log_values)) - lower
  #This stores max values for each section
  maxes = [0] * num_sections
  #This stores min values for each section
  mins = [0] * num_sections
  #This stores the highest color fraction for each section. This is calculated
  #proportionately to the number of vals in the section so larger sections have
  #more expressibility than smaller sections
  max_fracs = [0] * num_sections
  #For each section, calculate or find the above values
  for i in range(num_sections):
    temp = [j for j in log_values if j >= i + lower and j < i + lower + 1]
    maxes[i] = math.pow(10, max(temp)) if len(temp) != 0 else 0
    mins[i] = math.pow(10, min(temp)) if len(temp) != 0 else 0
    max_fracs[i] = (len(temp) / len(log_values)) + (max_fracs[i - 1]
      if i != 0 else 0)

  #Create an array that store RGB values for each data point
  color_data = np.full((data_h, data_w, 3), 255, dtype=np.uint8)
  #These are the colors for each coloring 'tier'
  #(low) dark green, yellow, red (high)
  colors = [[0,71,0],[255,255,0],[255,0,0]]

  #Loop through each y value for each x value
  for y, vals in enumerate(tif.data):
    for x, val in enumerate(vals):
      #Check if value is no data
      if val != tif.no_value and math.isfinite(val) and val > 0:
        #Get this val's section and the max and min values in that section
        section = math.floor(math.log10(val)) - lower
        max_v = maxes[section]
        min_v = mins[section]
        #Normalize value between 0 and 1
        val_frac = ((val - min_v) / (max_v - min_v) if max_v != min_v
                   else .5);
        #Get lower and upper bounds for this section's colors
        lb = 0 if section == 0 else max_fracs[section - 1]
        ub = max_fracs[section]
        #Normalize between lb and ub
        val_frac = (val_frac * (ub - lb)) + lb
        #write color value to array, inputted as [row, col]
        color_data[y, x] = tif.getHeatMapColor(colors, val_frac)
    print ("\rCreating heatmap {}%".format(
           round(y*100/(data_h-1))), end="")

  #Add space for the legend
  #Height of each line of the legend is the image height / 30
  line_h = math.floor(data_h / 30)
  #Create font using font file in etc/ folder
  path_to_here = os.path.dirname(__file__)
  path_to_here += "/" if path_to_here != "" else ""
  font_type = path_to_here + "../etc/times-new-roman.ttf"
  font = ImageFont.truetype(font_type, line_h)
  #Width of the legend is the width of the text * 2 for the gradient
  test_text = "  " + min_str + max_str
  legend_w = font.getsize(test_text)[0] * 2
  #Height of the legend is one line + 1.2 for 1 space
  legend_h = math.floor(line_h * 2.2)
  #Append rows for the legend
  extra_rows = np.full((legend_h, data_w, 3), 255, dtype=np.uint8)
  color_data = np.vstack((color_data, extra_rows))
  #Append columns for the legend
  if data_w < legend_w:
    extra_columns = np.full((data_h + legend_h,
                            math.floor((legend_w - data_w) / 2), 3),
                            255, dtype=np.uint8)
    #Add to start and end
    color_data = np.hstack((extra_columns, color_data, extra_columns))

  #Write data to image
  img = Image.fromarray(color_data, 'RGB')
  draw = ImageDraw.Draw(img)
  #Calculate vertical spacing and sample gradient length
  vert_space = math.floor(line_h / 5)
  grad_len = math.floor(legend_w / 2)
  #Get half the verticle offset of text to center text wth gradient
  vert_offset = math.floor(font.getoffset(test_text)[1] / 2)
  
  #Write min value into image
  x_offset = 0 if data_w <= legend_w else math.floor((data_w - legend_w) / 2)
  draw.text((x_offset, data_h + line_h - vert_offset),
            min_str + ' ', (0,0,0), font=font)
  
  #Draw color gradient into image
  x_offset += font.getsize(min_str + ' ')[0]
  #As we go along the gradient, store which range the corresponding number lies
  #in as well as the lower and upper bounds of that range
  section = 0
  num_ub = (math.pow(10, section + lower + 1) if
            section != len(max_fracs) - 1 else max_val)
  num_lb = min_val
  for i in range(grad_len + 1):
    #Get the number this slice of the gradient represents
    frac = ((i / grad_len) * (max_val - min_val)) + min_val
    #Make sure we are in the correct power of ten range
    while section < len(max_fracs) - 1:
      if frac >= num_ub:
        section += 1
        #Update our lower and upper bounds
        num_lb = num_ub
        num_ub = (math.pow(10, section + lower + 1) if
                  section != len(max_fracs) - 1 else max_val)
      else:
        break
    #Calculate how far from the lower bound to the upper bound our value is
    frac = (frac - num_lb) / (num_ub - num_lb)
    #Get the upper and lower bounds for our color
    color_ub = max_fracs[section]
    color_lb = max_fracs[section - 1] if section != 0 else 0
    #Calculate the color fraction for our value
    frac = (frac * (color_ub - color_lb) + color_lb if
            color_ub != color_lb else 1)
    #Get current color
    color = tif.getHeatMapColor(colors, frac)
    #Draw a vertical line, width = 1px (1 col), height = legend height
    draw.line((x_offset + i, data_h + line_h, x_offset + i,
              data_h + (2 * line_h)), fill=color)
  
  #Write max value into image
  x_offset += i
  draw.text((x_offset, data_h + line_h - vert_offset),
            ' ' + max_str, (0,0,0), font=font)
  #Save image
  img.save(path + tif.file_name[:-4] + '.png')

#Run it
main()
