import math
import random
from typing import List, Set, Dict, Tuple

class AdjecencyList:

	def __init__(self):
		self.m_edgeSet: Set[Tuple[str, str, float]] = set()

	def addEdge(self, fromVtx: str, toVtx: str, weight: float):
		self.m_edgeSet.add((fromVtx, toVtx, weight))

class AdjacencyMatrix:

    def __init__(self, vtxCount: int):
        self.m_vtxCount = vtxCount
        self.m_matrix: List[float] = [float] * vtxCount * vtxCount

    def vertexCount(self) -> int:
        return self.m_vtxCount

    def weightOfEdge(self, startVtx: int, endVtx: int) -> float:
        return self.m_matrix[startVtx * self.m_vtxCount + endVtx]

class Graph:

	def __init__(self, edges: AdjecencyList):
		self.vertexAlias: Dict[str, int] = {}
		self.vertexTable: List[str] = []

		weightTable: List[Tuple[int, int, float]] = []
		# Accessing member... for now
		for fromVtx, toVtx, weight in edges.m_edgeSet:

			if fromVtx not in self.vertexAlias:
				self.vertexAlias[fromVtx] = len(self.vertexTable)
				self.vertexTable.append(fromVtx)
			
			if toVtx not in self.vertexAlias:
				self.vertexAlias[toVtx] = len(self.vertexTable)
				self.vertexTable.append(toVtx)

			weightTable.append((self.vertexAlias[fromVtx], self.vertexAlias[toVtx], weight))

		self.adjacencyMatrix = AdjacencyMatrix(len(self.vertexTable))
		for x, y, w in weightTable:
			self.adjacencyMatrix.m_matrix[x + y * len(self.vertexTable)] = w

	def vertecies(self) -> Set[str]:
		return set(self.vertexTable)
	
	def vertexCount(self) -> int:
		return self.adjacencyMatrix.vertexCount()
	
	def weightOfEdge(self, startName: str, endName: str) -> float:
		return self.adjacencyMatrix.weightOfEdge(
			self.vertexAlias[startName], self.vertexAlias[endName])