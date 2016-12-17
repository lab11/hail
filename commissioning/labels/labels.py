#!/usr/bin/env python

"""
Generate pdfs of labels for Triumvi Cases
"""

import os
import sys
import svgutils.transform as sg


print(sys.executable)

import sh
from sh import pdf2svg
from sh import rsvg_convert

# MAIN_LABEL_PDF = 'case_label.pdf'
# MAIN_LABEL_SVG = 'case_label.svg'

# MAIN_LABEL_PDF = 'logo.pdf'
MAIN_LABEL_SVG = 'label.svg'

DEVICE_ID = 'C0:98:E5:13:'


POSITION_START_X = 0
POSITION_START_Y = 0
x = POSITION_START_X
y = POSITION_START_Y

# label_specs = {}
# label_specs['offset_x'] = 34
# label_specs['gap_x']    = 8.5
# label_specs['width_x']  = 54
# label_specs['offset_y'] = 34.5
# label_specs['gap_y']    = 0
# label_specs['height_y'] = 72
# label_specs['y_count']  = 10
# label_specs['x_count']  = 9

label_specs = {}
label_specs['offset_x'] = 18
label_specs['gap_x']    = 0
label_specs['width_x']  = 72
label_specs['offset_y'] = 36
label_specs['gap_y']    = 0
label_specs['height_y'] = 36
label_specs['y_count']  = 20
label_specs['x_count']  = 8

def get_coordinates ():
	global x, y

	xpx = label_specs['offset_x'] + (x*(label_specs['gap_x'] + label_specs['width_x']))
	ypx = label_specs['offset_y'] + (y*(label_specs['gap_y'] + label_specs['height_y']))

	x += 1

	if x > label_specs['x_count']-1:
		x = 0
		y += 1

	return (round(xpx), round(ypx))

# List of ids to make QR codes of
ids = []

if len(sys.argv) != 3:
	print('Usage: {} <start id in hex> <number of labels>'.format(__file__))
	print('example: {} 00f0 16'.format(__file__))
	sys.exit(1)

start_id = int(sys.argv[1], 16)
count = int(sys.argv[2])


for id in range(start_id, start_id+count):
	first = id >> 8
	second = id & 0xff

	label_id = '{}{:02X}:{:02X}'.format(DEVICE_ID, first, second)
	ids.append(label_id)

print(ids)


if len(ids) == 0:
	print('No IDs to make labels for!')
	sys.exit(1)


ltype = 'sc'
# label_pdf = MAIN_LABEL_PDF
label_svg = MAIN_LABEL_SVG
label_pixels_x = 72
label_pixels_y = 36
label_id_pos_x = 36
label_id_pos_y = 30
label_id_font  = 7.5
label_id_letterspacing = -0.7
# label_rotate = True
label_rotate = False


label_sheet = sg.SVGFigure('612', '792') # 8.5"x11" paper at 72dpi
# label_sheet = sg.SVGFigure('792', '612') # 8.5"x11" paper at 72dpi
labels = []

# Convert the base label pdf to svg
# pdf2svg(label_pdf, label_svg)

background = sg.fromfile('label_back.svg')
backgroundr = background.getroot()
# pos = get_coordinates()
# 	# print(pos)
# 	lblr.moveto(pos[0], pos[1], 1) # position correctly (hand tweaked)
# 	# lblr.set_size(['{}pt'.format(label_pixels_x), '{}pt'.format(label_pixels_y)])
labels.append(backgroundr)


for nodeid in ids:
	nodeidstr = nodeid.replace(':', '')

	# Create the node specific svg
	fig = sg.SVGFigure(width='{}px'.format(label_pixels_x), height='{}px'.format(label_pixels_y))

	rawlabel = sg.fromfile(label_svg)
	# print(rawlabel.get_size())
	# rawlabel.set_size(['{}px'.format(label_pixels_x), '{}px'.format(label_pixels_y)])
	# print(rawlabel.get_size())
	rawlabelr = rawlabel.getroot()

	#txt = sg.TextElement(100,318, nodeid, size=28, font='Courier')
	txt = sg.TextElement(label_id_pos_x,
	                     label_id_pos_y, nodeid,
	                     anchor='middle',
	                     size=label_id_font,
	                     font='Courier',
	                     letterspacing=label_id_letterspacing,
	                     color='white')
	fig.append([rawlabelr, txt])
	# fig.append([rawlabelr])
	# print(fig.to_str())
	fig.save('label_{}.svg'.format(nodeidstr))

	if label_rotate:
		fig = sg.SVGFigure('{}px'.format(label_pixels_y), '{}px'.format(label_pixels_x))
		dlabel = sg.fromfile('label_{}.svg'.format(nodeidstr))
		dlabelr = dlabel.getroot()
		dlabelr.rotate(90, x=0, y=0)
		dlabelr.moveto(0, -1*label_pixels_y)
		fig.append([dlabelr])
		fig.save('label_{}.svg'.format(nodeidstr))

#	labels.append(fig)

	# Convert the id specific image to pdf
#	sh.rsvg_convert('-f', 'pdf', '-o', 'label_{}.pdf'.format(nodeidstr),
#		'label_{}.svg'.format(nodeidstr))

	# Stamp the label with id specific image
#	pdftk(CASE_LABEL, 'stamp', 'unique_{}.pdf'.format(nodeidstr), 'output',
#		'label_{}.pdf'.format(nodeidstr))

#	pdf2svg('label_{}.pdf'.format(nodeidstr), 'label_{}.svg'.format(nodeidstr))

	lbl = sg.fromfile('label_{}.svg'.format(nodeidstr))
	# lbl.set_size(['{}pt'.format(label_pixels_x), '{}pt'.format(label_pixels_y)])
	lblr = lbl.getroot()
	pos = get_coordinates()
	# print(pos)
	lblr.moveto(pos[0], pos[1], 1) # position correctly (hand tweaked)
	# lblr.set_size(['{}pt'.format(label_pixels_x), '{}pt'.format(label_pixels_y)])

	labels.append(lblr)

label_sheet.append(labels)
# print(label_sheet.to_str())
base_name = 'hail_{}_{}'.format(sys.argv[1], sys.argv[2])
label_sheet.save('{}.svg'.format(base_name))
sh.rsvg_convert('-f', 'pdf', '-d', '72', '-p', '72', '-o',
	'{}.pdf'.format(base_name), '{}.svg'.format(base_name))
print('{}.pdf'.format(base_name))










