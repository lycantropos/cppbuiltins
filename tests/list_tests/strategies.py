import sys
from typing import (Any,
                    Callable,
                    List,
                    Tuple)

from hypothesis import strategies

from tests.utils import (AlternativeNativeListsPair,
                         Domain,
                         Strategy,
                         to_alternative_native_lists_pair)

MAX_INDEX = sys.maxsize
MIN_INDEX = -MAX_INDEX - 1
indices = strategies.integers(MIN_INDEX, MAX_INDEX)
sizes = strategies.integers(0, min(100, sys.maxsize))
invalid_sizes = strategies.integers(MIN_INDEX, -1)
objects = strategies.integers()
empty_lists = strategies.builds(list)
objects_lists = strategies.lists(objects)
non_empty_objects_lists = strategies.lists(objects,
                                           min_size=1)
lists_pairs = strategies.builds(to_alternative_native_lists_pair,
                                objects_lists)
empty_lists_pairs = strategies.builds(to_alternative_native_lists_pair,
                                      empty_lists)
non_empty_lists_pairs = strategies.builds(to_alternative_native_lists_pair,
                                          non_empty_objects_lists)


def to_non_empty_lists_pairs_with_their_indices(
        pair: AlternativeNativeListsPair
) -> Strategy[Tuple[AlternativeNativeListsPair, int]]:
    alternative, _ = pair
    size = len(alternative)
    return strategies.tuples(strategies.just(pair),
                             strategies.integers(-size, size - 1))


non_empty_lists_pairs_with_their_indices = non_empty_lists_pairs.flatmap(
        to_non_empty_lists_pairs_with_their_indices)


def to_non_empty_lists_pairs_with_their_elements(
        values: List[Any]) -> Strategy[Tuple[AlternativeNativeListsPair, Any]]:
    pair = to_alternative_native_lists_pair(values)
    return strategies.tuples(strategies.just(pair),
                             strategies.sampled_from(values))


non_empty_lists_pairs_with_their_elements = non_empty_objects_lists.flatmap(
        to_non_empty_lists_pairs_with_their_elements)


def to_lists_pairs_with_non_their_elements(
        values: List[Any]) -> Strategy[Tuple[AlternativeNativeListsPair, Any]]:
    pair = to_alternative_native_lists_pair(values)
    return strategies.tuples(strategies.just(pair),
                             objects.filter(lambda candidate
                                            : candidate not in values)
                             if values
                             else objects)


lists_pairs_with_non_their_elements = (
    objects_lists.flatmap(to_lists_pairs_with_non_their_elements))


@strategies.composite
def to_non_empty_lists_pairs_with_starts_stops_and_their_elements(
        draw: Callable[[Strategy[Domain]], Domain],
        values: List[Any]
) -> Strategy[Tuple[AlternativeNativeListsPair, int, int, Any]]:
    pair = to_alternative_native_lists_pair(values)
    size = len(values)
    start = draw(strategies.integers(MIN_INDEX, size - 1))
    min_positive_stop = max(-size, start) % size + 1
    stops = strategies.integers(min_positive_stop, MAX_INDEX)
    stop = draw(stops | strategies.integers(min_positive_stop % -size, -1)
                if min_positive_stop < size
                else stops)
    return pair, start, stop, draw(strategies.sampled_from(values[start:stop]))


non_empty_lists_pairs_with_starts_stops_and_their_elements = (
    non_empty_objects_lists.flatmap(
            to_non_empty_lists_pairs_with_starts_stops_and_their_elements))


@strategies.composite
def to_lists_pairs_with_starts_stops_and_non_their_elements(
        draw: Callable[[Strategy[Domain]], Domain],
        values: List[Any]
) -> Strategy[Tuple[AlternativeNativeListsPair, int, int, Any]]:
    pair = to_alternative_native_lists_pair(values)
    start, stop = draw(indices), draw(indices)
    sub_values = values[start:stop]
    return (pair, start, stop,
            draw(objects.filter(lambda candidate: candidate not in sub_values)
                 if sub_values
                 else objects))


lists_pairs_with_starts_stops_and_non_their_elements = (
    (objects_lists
     .flatmap(to_lists_pairs_with_starts_stops_and_non_their_elements)))


def to_lists_pairs_with_invalid_indices(
        pair: AlternativeNativeListsPair
) -> Strategy[Tuple[AlternativeNativeListsPair, int]]:
    alternative, _ = pair
    size = len(alternative)
    return strategies.tuples(strategies.just(pair),
                             strategies.integers(MIN_INDEX, -size - 1)
                             | strategies.integers(size + 1, MAX_INDEX))


lists_pairs_with_invalid_indices = lists_pairs.flatmap(
        to_lists_pairs_with_invalid_indices)


def to_lists_pairs_with_slices(
        pair: AlternativeNativeListsPair
) -> Strategy[Tuple[AlternativeNativeListsPair, slice]]:
    alternative, _ = pair
    size = len(alternative)
    return strategies.tuples(strategies.just(pair), strategies.slices(size))


lists_pairs_with_slices = (lists_pairs
                           .flatmap(to_lists_pairs_with_slices))


@strategies.composite
def to_lists_pairs_with_slices_and_iterables_pairs(
        draw: Callable[[Strategy[Domain]], Domain],
        pair: AlternativeNativeListsPair
) -> Strategy[Tuple[AlternativeNativeListsPair, slice, List[Any]]]:
    alternative, _ = pair
    size = len(alternative)
    slice_ = draw(strategies.slices(size))
    slice_size = len(alternative[slice_])
    return pair, slice_, draw((objects_lists
                               if slice_.step == 1
                               else strategies.lists(objects,
                                                     min_size=slice_size,
                                                     max_size=slice_size))
                              if slice_size
                              else empty_lists)


lists_pairs_with_slices_and_objects_lists = lists_pairs.flatmap(
        to_lists_pairs_with_slices_and_iterables_pairs)
