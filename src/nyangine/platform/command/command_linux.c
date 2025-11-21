#include <sys/types.h>
#include <sys/wait.h>

#include "nyangine/nyangine.h"

void nya_command_run(NYA_Command* command) {
  nya_assert(command);
  nya_assert(command->arena);
  nya_assert(command->program);

  u64 start_time = nya_clock_get_unix_timestamp_ms();

  command->stdout_content = nya_string_new(command->arena);
  command->stderr_content = nya_string_new(command->arena);

  s32 stdout_pipe[2];
  s32 stderr_pipe[2];
  if (pipe(stdout_pipe) != 0 || pipe(stderr_pipe) != 0) {
    command->exit_code = 255;
    return;
  }

  pid_t pid = fork();
  if (pid < 0) {
    command->exit_code = 255;
    return;
  }

  // CHILD
  if (pid == 0) {
    // redirect stdout and stderr
    dup2(stdout_pipe[1], STDOUT_FILENO);
    dup2(stderr_pipe[1], STDERR_FILENO);
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[0]);
    close(stderr_pipe[1]);

    // change working directory
    if (command->working_directory != nullptr && strlen(command->working_directory) != 0) {
      if (chdir(command->working_directory) != 0) {
        perror("chdir");
        _exit(1);
      }
    }

    // set environment variables
    for (u32 i = 0; i < nya_carray_length(command->environment); i++) {
      if (command->environment[i] == nullptr) break;
      putenv(command->environment[i]);
    }

    // build argv
    NYA_ConstCString* argv = nya_alloca(sizeof(NYA_CString) * (2 + nya_carray_length(command->arguments)));
    argv[0]                = command->program;
    nya_memcpy(argv + 1, command->arguments, nya_carray_length(command->arguments) * sizeof(NYA_ConstCString));
    argv[1 + nya_carray_length(command->arguments)] = nullptr;

    // do the thing
    execvp(command->program, (char* const*)argv);
    perror("execvp");
    _exit(1);
  }

  // PARENT
  close(stdout_pipe[1]);
  close(stderr_pipe[1]);

  // read stdout and stderr
  nya_fd_read(stdout_pipe[0], &command->stdout_content);
  nya_fd_read(stderr_pipe[0], &command->stderr_content);
  close(stdout_pipe[0]);
  close(stderr_pipe[0]);

  // read exit code
  waitpid(pid, &command->exit_code, 0);

  u64 end_time               = nya_clock_get_unix_timestamp_ms();
  command->execution_time_ms = end_time - start_time;
}

void nya_command_destroy(NYA_Command* command) {
  nya_assert(command);

  nya_string_destroy(&command->stdout_content);
  nya_string_destroy(&command->stderr_content);
}
