from typing import (Any,
                    List)

from hypothesis import given

from tests.utils import (AlternativeList,
                         NativeList,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.objects_lists)
def test_basic(objects: List[Any]) -> None:
    alternative, native = AlternativeList(objects), NativeList(objects)

    assert are_alternative_native_lists_equal(alternative, native)
