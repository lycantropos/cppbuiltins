import builtins
import pickle
from operator import eq
from typing import (Any,
                    List,
                    Tuple,
                    TypeVar)

import cppbuiltins
from hypothesis.strategies import SearchStrategy as Strategy

Domain = TypeVar('Domain')
Strategy = Strategy
AlternativeList = cppbuiltins.list
NativeList = builtins.list
AlternativeNativeListsPair = Tuple[AlternativeList, NativeList]


def equivalence(left: bool, right: bool) -> bool:
    return left is right


def pickle_round_trip(value: Domain) -> Domain:
    return pickle.loads(pickle.dumps(value))


def are_alternative_native_lists_equal(alternative: AlternativeList,
                                       native: NativeList) -> bool:
    return (len(alternative) == len(native)
            and all(map(eq, alternative, native)))


def to_alternative_native_lists_pair(values: List[Any]
                                     ) -> AlternativeNativeListsPair:
    return AlternativeList(values), NativeList(values)
