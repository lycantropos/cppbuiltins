from hypothesis import strategies

from tests.utils import AlternativeSet

objects = strategies.integers()
objects_sets = strategies.sets(objects)
sets = objects_sets.map(AlternativeSet)
