import builtins
import fractions
import math
import pickle
from itertools import zip_longest
from operator import eq
from typing import (Any,
                    Hashable,
                    Iterator,
                    List,
                    Optional,
                    Tuple,
                    TypeVar)

import cppbuiltins
from hypothesis.strategies import SearchStrategy as Strategy

Domain = TypeVar('Domain')
Range = TypeVar('Range')
Strategy = Strategy
AlternativeFraction = cppbuiltins.Fraction
AlternativeInt = cppbuiltins.int
AlternativeList = cppbuiltins.list
AlternativeSet = cppbuiltins.set
NativeFraction = fractions.Fraction
NativeInt = builtins.int
NativeList = builtins.list
NativeSet = builtins.set
AlternativeNativeFractionsPair = Tuple[AlternativeFraction, NativeFraction]
AlternativeNativeIntsPair = Tuple[AlternativeInt, NativeInt]
AlternativeNativeListsPair = Tuple[AlternativeList, NativeList]
AlternativeNativeSetsPair = Tuple[AlternativeSet, NativeSet]

alternative_gcd = cppbuiltins.gcd
native_gcd = math.gcd


def are_iterators_equal(left: Iterator[Any],
                        right: Iterator[Any],
                        *,
                        _sentinel: Any = object()) -> bool:
    return all(left_value == right_value
               for left_value, right_value in zip_longest(left, right,
                                                          fillvalue=_sentinel))


def are_alternative_native_fractions_equal(alternative: AlternativeFraction,
                                           native: NativeFraction) -> bool:
    return (are_alternative_native_ints_equal(alternative.denominator,
                                              native.denominator)
            and are_alternative_native_ints_equal(alternative.numerator,
                                                  native.numerator))


def are_alternative_native_ints_equal(alternative: AlternativeInt,
                                      native: NativeInt) -> bool:
    return int(alternative) == native


def are_alternative_native_lists_equal(alternative: AlternativeList,
                                       native: NativeList) -> bool:
    return (len(alternative) == len(native)
            and all(map(eq, alternative, native)))


def are_alternative_native_sets_equal(alternative: AlternativeSet,
                                      native: NativeSet) -> bool:
    return (len(alternative) == len(native)
            and all(element in native for element in alternative)
            and all(element in alternative for element in native))


def equivalence(left: bool, right: bool) -> bool:
    return left is right


def pickle_round_trip(value: Domain) -> Domain:
    return pickle.loads(pickle.dumps(value))


def to_alternative_native_fractions_pair(numerators_pair
                                         : AlternativeNativeIntsPair,
                                         denominators_pair
                                         : Optional[AlternativeNativeIntsPair]
                                         = None
                                         ) -> AlternativeNativeFractionsPair:
    alternative_numerator, native_numerator = numerators_pair
    if denominators_pair is None:
        return (AlternativeFraction(alternative_numerator),
                NativeFraction(native_numerator))
    alternative_denominator, native_denominator = denominators_pair
    return (AlternativeFraction(alternative_numerator,
                                alternative_denominator),
            NativeFraction(native_numerator, native_denominator))


def to_alternative_native_ints_pair(native: int) -> AlternativeNativeIntsPair:
    return AlternativeInt(native), native


def to_alternative_native_lists_pair(values: List[Any]
                                     ) -> AlternativeNativeListsPair:
    return AlternativeList(values), NativeList(values)


def to_alternative_native_sets_pair(values: List[Hashable]
                                    ) -> AlternativeNativeSetsPair:
    return AlternativeSet(values), NativeSet(values)
