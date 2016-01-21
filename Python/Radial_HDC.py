import sys
import time
import copy

class Cluster:
    
    def __init__(self, element_name, cluster_type, newickstring):
        # initializes the data members
        self.element_name = element_name
        self.cluster_type = cluster_type
        self.newickstring = newickstring
        self.pos_list = []
        self.child_list = []
        self.cluster_list = []
        self.radius = -1.0

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
		
		new_cluster = Cluster(split_line[0],"LEAF",split_line[0])
		#new_cluster.pos_x = split_line[1]
		#new_cluster.pos_y = split_line[2]
		new_cluster.pos_list = data[line_counter] #ADDED BY CRHODE (09272014)
		cluster_list.append(new_cluster)		
		
		line_counter += 1

#def formDistanceMatrixFromClustersALL():
#	#print("Recalculating Distance Matrix Using Clusters (full square matrix)")
#	
#	total_rows = len(cluster_list)
#	
#	for c1 in range(0,total_rows):
#		dist_matrix.append([])
#		
#		for c2 in range(0,total_rows):
#			ed = euclidDistanceWithLists(cluster_list[c1].pos_list, cluster_list[c2].pos_list)
#			dist_matrix[c1].append(ed)
			
def formDistanceMatrix(cluster, dist_matrix):
	max_dist = -1
	total_rows = len(cluster.cluster_list)
	
	for c1 in range(0,total_rows):
		dist_matrix.append([])
		
		for c2 in range(0,total_rows):
			ed = euclidDistanceWithLists(cluster.cluster_list[c1].pos_list, cluster.cluster_list[c2].pos_list)
			dist_matrix[c1].append(ed)
			
			if(ed > max_dist):
				max_dist = ed
				
	return max_dist

def euclidDistanceWithLists(list1, list2):
	temp = 0
	size_list = len(list1)
	
	for list_index in range(0,size_list):
		temp += abs(list1[list_index] - list2[list_index])**2
	
	return float(temp**0.5)

######################
#RADIAL NEIGHBOR LIST
######################
def formRadialNeighborList(dist_matrix, neighbor_list, minR):
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

def getIndexWithLeastNeighbors(neighbor_list):
	index_most_neighbors = -1;
	most_neighbors = 99999999;
	
	for list_index in range(0,len(neighbor_list)):
		if(neighbor_list[list_index][0] == -1):
			continue
		
		if(len(neighbor_list[list_index]) - 1 < most_neighbors):
			index_most_neighbors = list_index
			most_neighbors = len(neighbor_list[list_index]) - 1
	
	return index_most_neighbors

def printTree(cluster):
	counter = 1
	for i in range(0,len(cluster.child_list)):
		print "Child" + str(counter)
		counter+=1
		for j in range(0,len(cluster.child_list[i].cluster_list)):
			print "  " + cluster.child_list[i].cluster_list[j].element_name

def printTreeCurrent(cluster):
	for i in range(0,len(cluster.cluster_list)):
		print "  " + cluster.cluster_list[i].element_name + str(cluster.cluster_list[i].pos_list)

#
# Radial Divisive Clustering
#
# Take current cluster and do the following:
#   1) figure out distance matrix for current cluster
#   2) form neighbor list from neighbor list using the radius argument passed in
#   3) cluster observation with SMALLEST neighbor list -> reform list -> repeat
#   4) stop when list is empty or list contains only observations with no neighbors at all
#
def divisiveClustering(cluster, radius, radius_shift):
	#####print("-------------------------------------------------------")
	#####printTreeCurrent(cluster)
	
	
	if(len(cluster.cluster_list) == 1):
		return

	#####print "Radius = " + str(radius)
	dist_matrix = []
	neighbor_list = []
	
	#####print "Before forming distance matrix, it's now ..."
	#####print(dist_matrix)
	max_dist = formDistanceMatrix(cluster, dist_matrix)
	#####print(dist_matrix)
	
	while(radius >= max_dist):
		#####print ("Radius (" + str(radius) + ") >= max distance measure (" + str(max_dist) + ") ... decrementing radius")
		radius = radius + radius_shift
	
	#if(radius < 0.0):
	#	radius = 0.0
	
	formRadialNeighborList(dist_matrix, neighbor_list, radius)
	#####print "NEIGHBOR LIST"
	#####print(neighbor_list)
	
	index_least_neighbors = getIndexWithLeastNeighbors(neighbor_list)
	#####print "LEAST INDEX + NEIGHBOR LIST AT THAT INDEX"
	#####print(index_least_neighbors)
	#####print(neighbor_list[index_least_neighbors])
	
	#For shortest neighbor list found above, create a new cluster, assign it as child of parent cluster
	while index_least_neighbors != -1:
		new_cluster = Cluster("","CLUSTER","")
		
		if(radius < 0.0):
			new_cluster.radius = 0.0
		else:
			new_cluster.radius = radius
		
		del(new_cluster.cluster_list[:])
		for ind in neighbor_list[index_least_neighbors]:
			new_cluster.cluster_list.append(cluster.cluster_list[ind])
			#####print len(new_cluster.cluster_list)
		
		#####print"test0"
		#####print len(new_cluster.cluster_list)
		
		cluster.child_list.append(new_cluster)
		#####print "current num children = " + str(len(cluster.child_list))
	
		#For clustered nodes, blank out their lists in the neighbor_list (assign a -1 as a neighbor to indicate list is bad)
		delete_list = sorted(neighbor_list[index_least_neighbors],reverse=True)
		#print delete_list
		for del_index in delete_list:
			for x in reversed(range(0,len(neighbor_list[del_index]))):
				del(neighbor_list[del_index][x])
			neighbor_list[del_index].append(-1)
			#print(neighbor_list[del_index])
			#cluster_del_list.append(del_index)	
	
		#####print neighbor_list
	
		#For every list remaining, delete members that were just clustered (nodes can't belong to more than one cluster)
		for row in reversed(range(0,len(neighbor_list))):
			for x in reversed(range(0,len(neighbor_list[row]))):
				temp_value = neighbor_list[row][x]
				for del_index in delete_list:
					if(temp_value == del_index):
						del(neighbor_list[row][x])
	
		#####print neighbor_list
		
		index_least_neighbors = getIndexWithLeastNeighbors(neighbor_list)
		#####print()
		#####print(index_least_neighbors)
		#####print(neighbor_list[index_least_neighbors])
		
		#index_least_neighbors = -1
	
	del(dist_matrix[:])
	del(neighbor_list[:])
	
	if(radius < 0):
		cluster.radius = 0
	else:
		cluster.radius = radius
		
	#####print("---Printing clusters for this level---")
	#####printTree(cluster)
	#####print("---Finished printing clusters for this level---")
	
	for c in cluster.child_list:
		new_radius = radius + radius_shift
		divisiveClustering(c, new_radius, radius_shift)

def formNewickString(cluster):
	
	if(len(cluster.child_list) == 0):
		cluster.newickstring = cluster.cluster_list[0].newickstring
		return
	
	for c in cluster.child_list:
		formNewickString(c)
	
	temp_string = ""
	for l in cluster.child_list:
		temp_string += l.newickstring + ","
	cluster.newickstring = "(" + temp_string[:-1] + "):" + str(cluster.radius)

#################################
#            MAIN
#################################		

row_names = []
data = []
cluster_list = []

#Read in dataset
readData(sys.argv[1],sys.argv[2]) #forms cluster list here of initial clusters
#print(row_names)
#print(data)

t0 = time.time()

#Create initial (head node) cluster.  Assign all indicies of every possible observation
head_node = Cluster("","HEAD","")
head_node.cluster_list = cluster_list #read in from "readData" function

divisiveClustering(head_node,3.0,-0.25)

print time.time() - t0, "seconds to cluster"

formNewickString(head_node)

print("Forming Newick String ...")

print(head_node.newickstring + ";")