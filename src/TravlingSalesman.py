import math
from typing import List, Set
import random
import Graph

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
	graph = loadCSV()
	iter = 0
	min = costOfRoute(graph, generateRouteInitialRouteFor(graph))
	while True:
		route = generateRouteInitialRouteFor(graph)
		cost = costOfRoute(graph, route)
		if cost < min:
			min = cost
			print(iter, " New minimum found: ", cost, route.nodeSeq)
		iter += 1

def loadCSV() -> Graph.Graph:
	file = open("sample/ulysses16.csv", 'r')

	# Skip headers
	file.readline()
	file.readline()
	file.readline()

	nodes = []
	for line in file:
		nodes.append(tuple(line.split(',')))

	edges = Graph.AdjecencyList()
	for i, v1 in enumerate(nodes):
		for j, v2 in enumerate(nodes):
			id1, x1, y1 = v1
			id2, x2, y2 = v2
			
			xDiff = float(x1) - float(x2)
			yDiff = float(y1) - float(y2)

			edges.addEdge(id1, id2, math.sqrt(xDiff * xDiff + yDiff * yDiff))

	file.close()
	return Graph.Graph(edges)

def findRoute(graph: Graph.Graph):
	route = generateRouteInitialRouteFor(graph)
	for neighbourRoute in []:
		return 0


def costOfRoute(graph: Graph.Graph, route: Route) -> float:
	if route.stepCount() != graph.vertexCount():
		return None

	totalCost: float = 0
	for nodeA, nodeB in zip(route.nodeSeq, route.nodeSeq[1:]):
		edgeCost = graph.weightOfEdge(nodeA, nodeB)
		if edgeCost == 0:
			# This is not a valid edge
			return None
		totalCost += edgeCost

    # Complete the cycle Last -> First
	return totalCost + graph.weightOfEdge(route.startStep(), route.endStep())

def generateRouteInitialRouteFor(graph: Graph.Graph) -> Route:
	verticiesToVisit = list(graph.vertecies())
	random.shuffle(verticiesToVisit)
	route = Route()
	for vertex in verticiesToVisit:
		route.addStep(vertex)
	return route

def neighboutsOfRoute(route: Route) -> Set[Route]:
	routesteps = route.nodeSeq.copy()
	for i, n in enumerate(routesteps):
		for j, m in enumerate(routesteps[i + 1:]):
			routesteps[i], routesteps[i + j] = routesteps[i + j], routesteps[i]
			for 

	return None

if __name__ == "__main__":
	main()