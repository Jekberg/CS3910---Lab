import random
from typing import List, Dict

nodeNames: List[int] = ["A", "B", "C", "D"]
matrixSize: int = 4
adjecencyMatrix: List[int] = [
	0, 20, 42, 35, 
  	20, 0, 30, 34,
	42, 30, 0, 12,
	35, 42, 12, 0]


def isValidEdge(start: int, end: int) -> bool:
	return costOfEdge(start, end) is not None

def costOfEdge(start: int, end: int) -> int:
	cost: int = adjecencyMatrix[start * matrixSize + end]
	if cost == 0:
		return None
	return cost

class Route:

	npos = -1

	def __init__(self, *seq: int):

		# Replicate a LinkedHashSet.
		self.nodeToStep: Dict[int, int] = {}
		self.stepSeq: List[int] = []
		
		for nodeId in seq:
			self.addStep(nodeId)
	
	def addStep(self, nodeId: int) -> bool:
		
		if nodeId not in self.nodeToStep:
			self.nodeToStep[nodeId] = len(self.stepSeq)
			self.stepSeq.append(nodeId)
			return True
		else:
			return False

	def stepCount(self) -> int:
		return len(self.stepSeq)

	def startStep(self) -> int:
		if self.stepCount() > 0:
			return self.stepSeq[0]
		else:
			return npos

	def endStep(self) -> int:
		if self.stepCount() > 0:
			return self.stepSeq[self.stepCount() - 1]
		else:
			return npos
	
	def stepAt(self, stepNum: int) -> int:
		return self.stepSeq[stepNum]
	
	def seqOf(self, nodeId) -> int:
		return self.nodeToStep[nodeId]

def costOfRoute(route: Route) -> int:

	if route.stepCount() != matrixSize:
		return None
	
	totalCost: int = 0
	it = iter(route.stepSeq)

	now = next(it, None)	
	if now is None:
		return None

	while now is not None:

		moveTo = next(it, None)
		if moveTo is None:
			break

		edgeCost: int = costOfEdge(now, moveTo)
		if edgeCost is None:
			return None
		
		totalCost += edgeCost
		now = moveTo

	returnCost = costOfEdge(route.startStep(), route.endStep())
	if returnCost is None:
		return None

	return totalCost + returnCost

def randomRoute():

	startNode = 0
	nodesToVisit = [n for n in range(0, matrixSize)]
	route: Route = Route()

	route.addStep(startNode)
	nodesToVisit.remove(startNode)

	while len(nodesToVisit) != 0:

		node = random.choice(nodesToVisit)
		if isValidEdge(route.endStep(), node):
			nodesToVisit.remove(node)
			route.addStep(node)
		
	return route

while True:

	route = randomRoute()
	print(costOfRoute(route), route.stepSeq)
