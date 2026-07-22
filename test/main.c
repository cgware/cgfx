#include "log.h"
#include "mem.h"
#include "test.h"

STEST(gfx);
STEST(gfx_d3d11);
STEST(gfx_none);
STEST(gfx_opengl);
STEST(gfx_software);
STEST(gfx_vulkan);

TEST(cgfx)
{
	SSTART;
	RUN(gfx);
	RUN(gfx_d3d11);
	RUN(gfx_none);
	RUN(gfx_opengl);
	RUN(gfx_software);
	RUN(gfx_vulkan);
	SEND;
}

int main(int argc, char **argv)
{
	c_print_init();

	log_t log = {0};
	log_set(&log);
	log_add_callback(log_std_cb, DST_STD(), LOG_WARN, 1, 1);

	if (t_init(argc, argv)) {
		return 0;
	}

	t_run(test_cgfx, 1);

	int ret = t_finish();

	mem_print(DST_STD());

	if (mem_check()) {
		ret = 1;
	}

	return ret;
}
