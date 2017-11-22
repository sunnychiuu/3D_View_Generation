import os
import glob
import re
path = '/Users/SunnyC/Downloads/ModelNet40/*/*/*.off'
for data_path in glob.glob(path):
	data_class = data_path.split('/')[-3]
	data_dir = data_path.split('/')[-2]
	data_name = data_path.split('/')[-1]
    output_path = '/Users/SunnyC/Downloads/3d-view-gen/data_modelnet40/'+data_class+'/'+data_dir+'/'+data_name
	if not os.path.exists(output_path):
		os.makedirs(outputpath)
	#print data_name
	os.system('./main ' + data_path + ' ' +output_path)
#for data_name in os.listdir(data_dir):
	#os.system('./main ../train/'+data_name+' train_out')


