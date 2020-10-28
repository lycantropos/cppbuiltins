from typing import (Any,
                    List)

from hypothesis import given

from tests.utils import (AlternativeSet,
                         NativeSet,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.objects_lists)
def test_basic(objects: List[Any]) -> None:
    alternative, native = AlternativeSet(objects), NativeSet(objects)

    assert are_alternative_native_sets_equal(alternative, native)
