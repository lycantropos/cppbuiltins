import builtins
import pickle
from itertools import zip_longest
from operator import eq
from typing import (Any,
                    Hashable,
                    Iterator,
                    List,
                    Tuple,
                    TypeVar)

import cppbuiltins
from hypothesis.strategies import SearchStrategy as Strategy

Domain = TypeVar('Domain')
Range = TypeVar('Range')
Strategy = Strategy
AlternativeList = cppbuiltins.list
AlternativeSet = cppbuiltins.set
NativeList = builtins.list
NativeSet = builtins.set
AlternativeNativeListsPair = Tuple[AlternativeList, NativeList]
AlternativeNativeSetsPair = Tuple[AlternativeSet, NativeSet]


def equivalence(left: bool, right: bool) -> bool:
    return left is right


def pickle_round_trip(value: Domain) -> Domain:
    return pickle.loads(pickle.dumps(value))


def are_iterators_equal(left: Iterator[Any],
                        right: Iterator[Any],
                        *,
                        _sentinel: Any = object()) -> bool:
    return all(left_value == right_value
               for left_value, right_value in zip_longest(left, right,
                                                          fillvalue=_sentinel))


def are_alternative_native_lists_equal(alternative: AlternativeList,
                                       native: NativeList) -> bool:
    return (len(alternative) == len(native)
            and all(map(eq, alternative, native)))


def are_alternative_native_sets_equal(alternative: AlternativeSet,
                                      native: NativeSet) -> bool:
    return (len(alternative) == len(native)
            and all(element in native for element in alternative)
            and all(element in alternative for element in native))


def to_alternative_native_lists_pair(values: List[Any]
                                     ) -> AlternativeNativeListsPair:
    return AlternativeList(values), NativeList(values)


def to_alternative_native_sets_pair(values: List[Hashable]
                                    ) -> AlternativeNativeSetsPair:
    return AlternativeSet(values), NativeSet(values)
