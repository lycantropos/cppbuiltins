from typing import (Any,
                    List,
                    Tuple)

import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.non_empty_lists_pairs_with_their_indices, strategies.objects)
def test_index(pair_with_index: Tuple[AlternativeNativeListsPair, int],
               value: Any) -> None:
    (alternative, native), index = pair_with_index

    alternative[index] = native[index] = value

    assert are_alternative_native_lists_equal(alternative, native)


@given(strategies.lists_pairs_with_slices_and_objects_lists)
def test_slice(pair_with_slice_and_values
               : Tuple[AlternativeNativeListsPair, int, List[Any]]) -> None:
    (alternative, native), slice_, values = pair_with_slice_and_values

    alternative[slice_] = native[slice_] = values

    assert are_alternative_native_lists_equal(alternative, native)


@given(strategies.lists_pairs_with_non_their_indices, strategies.objects)
def test_invalid_index(pair_with_index: Tuple[AlternativeNativeListsPair, int],
                       value: Any) -> None:
    (alternative, native), index = pair_with_index

    with pytest.raises(IndexError):
        alternative[index] = value
    with pytest.raises(IndexError):
        native[index] = value
