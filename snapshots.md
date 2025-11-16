```C++
TEST(FlexboxTests, MyTest) {
	ResourceManager resources;
	Renderer renderer = Renderer::with_bitmap(width, height);

	// draw stuff
	renderer.draw(resources);

	EXPECT_SNAPSHOT_EQ(renderer.bitmap());
}
```

EXPECT_SNAPSHOT_EQ compares `bitmap` with snapshot file on disk.
If no file exists, EXPECT_SNAPSHOT_EQ passes and file is created.
If file exists, EXPECT_SNAPSHOT_EQ passes if `bitmap` and snapshot are equal.

If EXPECT_SNAPSHOT_EQ fails, both `bitmap` and snapshot are added to test report.
Test report is a HTML file that presents all snapshos with a diff for comparison.

- Where do we store the file?
  - The snapshot file needs to be stored somewhere we can commit them
  - The diff file needs to be stored somewhere we can gitignore, but still use in test report
- What are the file names going to be?
  - Probably based on the test suite + test case name
- How do we update snapshots?
  - Probably with a command line argument
- How do we handle multiple EXPECT_SNAPSHOT_EQ macros in same test?
  - Not seeing any real need for that right now, just show error message that it's not supported yet.

What should the test report look like?

Probably we need multiple HTML files, because we need to make it easy to check the diff on failure.
We can have a test summary in the index.html file, and then a <test-suite>.html file per test suite.
