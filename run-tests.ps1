docker-compose up --build --exit-code-from cppbuiltins

$STATUS = $LastExitCode

docker-compose down --remove-orphans

if ($STATUS -eq 0)
{
    echo "tests passed"
}
else
{
    echo "tests failed"
}

exit $STATUS
