from hypothesis import strategies

from tests.utils import AlternativeList

objects = strategies.integers()
objects_lists = strategies.lists(objects)
lists = objects_lists.map(AlternativeList)
