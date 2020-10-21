from typing import Tuple

from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.non_empty_lists_pairs_with_indices)
def test_index(pair_with_index: Tuple[AlternativeNativeListsPair, int]
               ) -> None:
    (alternative, native), index = pair_with_index

    del alternative[index], native[index]

    assert are_alternative_native_lists_equal(alternative, native)


@given(strategies.lists_pairs_with_slices)
def test_slice(pair_with_slice: Tuple[AlternativeNativeListsPair, int]
               ) -> None:
    (alternative, native), slice_ = pair_with_slice

    del alternative[slice_], native[slice_]

    assert are_alternative_native_lists_equal(alternative, native)
