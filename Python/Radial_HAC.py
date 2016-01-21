import sys
import time

class Cluster:
    child_list = [];
    element_name = "";
    cluster_type = "";
    newickstring = "";
    pos_x = float("inf")
    pos_y = float("inf")
    pos_list = [];
    
    def __init__(self, child_list, element_name, cluster_type, newickstring):
        # initializes the data members
        self.child_list.append(child_list)
        self.element_name = element_name
        self.cluster_type = cluster_type
        self.newickstring = newickstring

##########
#READ DATA
##########
def readData(filename, field_delim):
	f = open(filename,"r")
	line_counter = 0

	for line in f:
		if(line[0] == "#"):
			continue
			
		line = line.rstrip('\r\n')
		split_line = line.split(field_delim)
		item_counter = 0
		data.append([])
		
		for item in split_line:
			if(item_counter == 0):
				row_names.append(item)
				item_counter += 1
			else:
				data[line_counter].append(float(item))
		
		new_cluster = Cluster(None,split_line[0],"LEAF",split_line[0])
		#new_cluster.pos_x = split_line[1]
		#new_cluster.pos_y = split_line[2]
		new_cluster.pos_list = data[line_counter] #ADDED BY CRHODE (09272014)
		cluster_list.append(new_cluster)		
		
		line_counter += 1

################
#DISTANCE MATRIX
################
#def formDistanceMatrixURT():
#	#print("Calculating Distance Matrix (right upper triangle)")
#	
#	total_rows = len(row_names)
#	
#	for row1 in range(0,total_rows):
#		dist_matrix.append([])
#		dist_matrix[row1].append(0)
#		
#		for row2 in range(row1+1,total_rows):
#			x1,y1 = data[row1]
#			x2,y2 = data[row2]
#			ed = euclidDistance(x1,y1,x2,y2)
#			dist_matrix[row1].append(ed)
#			#print(str(data[row1][0]) + " " + str(data[row1][1]) + " | " + str(data[row2][0]) + " " + str(data[row2][1]) + " = " + str(ed))

def formDistanceMatrixFromClustersALL():
	#print("Recalculating Distance Matrix Using Clusters (full square matrix)")
	
	total_rows = len(cluster_list)
	
	for c1 in range(0,total_rows):
		dist_matrix.append([])
		
		for c2 in range(0,total_rows):
			ed = euclidDistanceWithLists(cluster_list[c1].pos_list, cluster_list[c2].pos_list)
			dist_matrix[c1].append(ed)
	
	#for row in cluster_list:
	#	print(row.element_name)
	#	print(row.pos_list)
	
	#for row in dist_matrix:
	#	print(row)

###################
#EUCLIDEAN DISTANCE
###################
#def euclidDistance(x1,y1,x2,y2):
#	temp = abs(x1 - x2)**2 + abs(y1 - y2)**2
#	return float(temp**0.5)
	
def euclidDistanceWithLists(list1, list2):
	temp = 0
	size_list = len(list1)
	
	for list_index in range(0,size_list):
		temp += abs(list1[list_index] - list2[list_index])**2
	
	return float(temp**0.5)
	
######################
#RADIAL NEIGHBOR LIST
######################
def formRadialNeighborList(minR):
	#print("Forming Radial Neighbor List -- Radius = " + str(minR))
	
	for row in range(0,len(neighbor_list)):
		del(neighbor_list[0])
	
	total_rows = len(dist_matrix)
	
	for row in range(0, total_rows):
		neighbor_list.append([])
		neighbor_list[row].append(row)
		for col in range(0, total_rows):
			if(dist_matrix[row][col] <= minR and row != col):
				neighbor_list[row].append(col)
		
		#print(str(row) + " - " + str(len(neighbor_list[row])))

def getIndexWithMostNeighbors():
	index_most_neighbors = -1;
	most_neighbors = 0;
	
	for list_index in range(0,len(neighbor_list)):
		if(len(neighbor_list[list_index]) - 1 > most_neighbors):
			index_most_neighbors = list_index
			most_neighbors = len(neighbor_list[list_index]) - 1
	
	return index_most_neighbors

#################################
#            MAIN
#################################		

#########################################
# Logic Idea
# 0) Read in Data
# 1) Form Distance Matrix
# 2) While we have more than a single cluster (since we're doing agglomerative/bottum up approach)
#    2a) From current neighbor list for current radius
#    2b) Find index of neighbor list that has the most neighbors at this radius
#    2c) While there exists indices with at least 1 neighbor
#        2c-1) Form a new cluster by adding all "neighbors" in that max index group as children clusters 
#        2c-2) add cluster to end of cluster_list
#        2c-3) For every observation/cluster, remove their existence from any non-clustered neighbor list
#        2c-4) Repeat clustering until this neighbor list (at this radius) is no nieghbors
#    2d) Remove all clusters used in (2c-1) from cluster list as they are now children of one of the clusters added in (2c-2)
#    2e) Reform distance matrix
#    2f) Repeat until our cluster_list is only a single, head cluster
#########################################

row_names = []
data = []
cluster_list = []
dist_matrix = []
neighbor_list = []

#Read in dataset
readData(sys.argv[1],sys.argv[2]) #forms cluster list here of initial clusters
#print(row_names)
#print(data)

t0 = time.time()

formDistanceMatrixFromClustersALL()

radius = 0.0

while len(cluster_list) > 1:
	
	radius = radius + 0.5
	
	#print("*************")
	#print("Radius = " + str(float(radius)))
	
	#FOR CURRENT DISTANCE MATRIX, FORM RADIAL NEIGHBOR LIST
	formRadialNeighborList(radius)
	
	cluster_del_list = []
	
	#FOR NEIGHBOR LIST, GET INDEX THAT HAS THE MOST NEIGHBORS
	index_most_neighbors = getIndexWithMostNeighbors()
	#print("index_most_neighbors = " + str(index_most_neighbors))
	
	#IF NO NEIGHBORS, START LOOP OVER AND TRY NEXT RADIUS (In this case don't reform distance matrix)
	if(index_most_neighbors == -1):
		#print("No neighbors at this radius.  Attempting next radius.")
		continue
	
	#WHILE WE CAN FIND SOME INDEX WITH AT LEAST 1 NEIGHBOR AT THIS RADIUS, FORM NEW CLUSTER
	#DELETE ROWS OF NEIGHBOR LIST FOR ALL CLUSTERED ELEMENTS, AND REMOVE SAID ELEMENTS FROM ALL REMAINING ROWS
	#REPEAT UNTIL NO MORE INDICES HAS ANY RADIAL NEIGHBORS
	while index_most_neighbors != -1:
		#for row in neighbor_list:
		#	print(row)
	
		new_cluster = Cluster(None,"","CLUSTER","")
		summed_x = float(0);
		summed_y = float(0);
		average_pos = [float(0)] * len(cluster_list[0].pos_list) #ADDED BY CRHODE(09272014)
	
		for neighbor_index in neighbor_list[index_most_neighbors]:
			#print("Attempting to add cluster (" + str(neighbor_index) + ") as a child node")
			new_cluster.child_list.append(cluster_list[neighbor_index])
			new_cluster.newickstring = new_cluster.newickstring + cluster_list[neighbor_index].newickstring + ","
			summed_x = summed_x + float(cluster_list[neighbor_index].pos_x)
			summed_y = summed_y + float(cluster_list[neighbor_index].pos_y)
			for p_index in range(0,len(average_pos)):
				average_pos[p_index] += cluster_list[neighbor_index].pos_list[p_index]
			#clustered_index_list.append(neighbor_index)
		new_cluster.pos_x = float(summed_x) / float(len(neighbor_list[index_most_neighbors]))
		new_cluster.pos_y = float(summed_y) / float(len(neighbor_list[index_most_neighbors]))
		for pos_index in range(0,len(average_pos)):
				average_pos[pos_index] = float(average_pos[pos_index]) / float(len(neighbor_list[index_most_neighbors]))
		new_cluster.pos_list = average_pos
		new_cluster.newickstring = "(" + new_cluster.newickstring[:-1] + ")" + ":" + str(float(radius))
		cluster_list.append(new_cluster)
		
		#print("Neighbor List before deleting")
		#print(neighbor_list)  
  
		delete_list = sorted(neighbor_list[index_most_neighbors],reverse=True)
		#print("Delete List Below ->")
		#print(delete_list)
		for del_index in delete_list:
			for x in reversed(range(0,len(neighbor_list[del_index]))):
				del(neighbor_list[del_index][x])
			neighbor_list[del_index].append(-1)
			#print(neighbor_list[del_index])
			cluster_del_list.append(del_index)	
	
		for row in reversed(range(0,len(neighbor_list))):
			for x in reversed(range(0,len(neighbor_list[row]))):
				temp_value = neighbor_list[row][x]
				for del_index in delete_list:
					if(temp_value == del_index):
						del(neighbor_list[row][x])
			
		#check to see if we continue looking for more clusters at this radius
		index_most_neighbors = getIndexWithMostNeighbors()
		#print("Reloop inner loop check -> index_most_neighbors = " + str(index_most_neighbors))
	
	#REMOVE CLUSTERS THAT WERE CLUSTERED DURING THIS SCAN
	cluster_del_list.sort(reverse=True)
	for del_ind in cluster_del_list:
		del(cluster_list[del_ind])
	
	#REFORM DISTANCE MATRIX FOR NEXT SCAN
	for row in range(0,len(dist_matrix)):
		del(dist_matrix[0])
	
	formDistanceMatrixFromClustersALL()

cluster_list[0].newickstring += ";"
		
#####################
#  POST LOGIC LOOP
#####################
print("-----------------------------------")
print("Clustering appears to have finished")
print("-----------------------------------")
print time.time() - t0, "seconds to cluster"
print(cluster_list[0].newickstring)
