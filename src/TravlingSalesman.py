import math
import random
from typing import List, Set, Dict

class AdjecencyList:

	def __init__(self):
		self.m_edgeSet: Set[Tuple[str, str, float]] = set()

	def addEdge(self, fromVtx: str, toVtx: str, weight: float):
		self.m_edgeSet.add((fromVtx, toVtx, weight))

class AdjacencyMatrix:

    def __init__(self, vtxCount: int):
        self.vtxCount= vtxCount
        self.matrix: List[float] = [0.0] * vtxCount * vtxCount

    def vertexCount(self) -> int:
        return self.vtxCount

    def weightOfEdge(self, startVtx: int, endVtx: int) -> float:
        return self.matrix[startVtx * self.vtxCount + endVtx]

class Graph:

	def __init__(self, edges: AdjecencyList):
		self.vertexAlias: Dict[str, int] = {}
		self.vertexTable: List[str] = []

		weightTable: List[Tuple[int, int, float]] = []
		# Accessing member... for now
		for edge in edges.m_edgeSet:
			fromVtx, toVtx, weight = edge

			if fromVtx not in self.vertexAlias:
				self.vertexAlias[fromVtx] = len(self.vertexTable)
				self.vertexTable.append(fromVtx)
			
			if toVtx not in self.vertexAlias:
				self.vertexAlias[toVtx] = len(self.vertexTable)
				self.vertexTable.append(toVtx)

			weightTable.append((self.vertexAlias[fromVtx], self.vertexAlias[toVtx], weight))

		self.adjacencyMatrix = AdjacencyMatrix(len(self.vertexTable))
		for x, y, w in weightTable:
			self.adjacencyMatrix.matrix[x + y * len(self.vertexTable)] = w

	def vertecies(self) -> Set[str]:
		return set(self.vertexTable)
	
	def vertexCount(self) -> int:
		return self.adjacencyMatrix.vertexCount()
	
	def weightOfEdge(self, startName: str, endName: str) -> float:
		return self.adjacencyMatrix.weightOfEdge(
			self.vertexAlias[startName], self.vertexAlias[endName])

theGraph: Graph = Graph(AdjecencyList())

class Route:

	def __init__(self, *nodeSeq: str):
		self.nodeSeq: List[str] = []
		for node in nodeSeq:
			self.addStep(node)
	
	def addStep(self, node: str) -> bool:
		if node not in self.nodeSeq:
			self.nodeSeq.append(node)
			return True
		else:
			return False

	def stepCount(self) -> int:
		return len(self.nodeSeq)

	def startStep(self) -> str:
		if self.stepCount() > 0:
			return self.nodeSeq[0]
		else:
			return None

	def endStep(self) -> str:
		if self.stepCount() > 0:
			return self.nodeSeq[self.stepCount() - 1]
		else:
			return None

def main():
	loadCSV()
	iter = 0
	min = costOfRoute(generateRandomRoute())
	while True:
		route = generateRandomRoute()
		cost = costOfRoute(route)
		if cost < min:
			min = cost
			print(iter, " New minimum found: ", cost, route.nodeSeq)
		iter += 1

def loadCSV():
	file = open("sample/ulysses16.csv", 'r')

	# Skip headers
	file.readline()
	file.readline()
	file.readline()

	nodes = []
	for line in file:
		nodes.append(tuple(line.split(',')))

	edges = AdjecencyList()
	for i, v1 in enumerate(nodes):
		for j, v2 in enumerate(nodes):
			id1, x1, y1 = v1
			id2, x2, y2 = v2
			
			xDiff = float(x1) - float(x2)
			yDiff = float(y1) - float(y2)

			edges.addEdge(id1, id2, math.sqrt(xDiff * xDiff + yDiff * yDiff))

	global theGraph
	theGraph = Graph(edges)
	file.close()

def costOfRoute(route: Route) -> float:
	if route.stepCount() != theGraph.vertexCount():
		return None

	totalCost: float = 0
	for nodeA, nodeB in zip(route.nodeSeq, route.nodeSeq[1:]):
		edgeCost = theGraph.weightOfEdge(nodeA, nodeB)
		if edgeCost == 0:
			# This is not a valid edge
			return None
		totalCost += edgeCost

    # Complete the cycle Last -> First
	return totalCost + theGraph.weightOfEdge(route.startStep(), route.endStep())

def generateRandomRoute() -> Route:
	verticiesToVisit = list(theGraph.vertecies())
	random.shuffle(verticiesToVisit)
	route = Route()
	for vertex in verticiesToVisit:
		route.addStep(vertex)
	return route

main()