#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <string.h>
#include <vector>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (21)

class Command;
class SmallShell;
class JobEntry {
    int job_id;
    std::string cmd_line;
    pid_t process_id;
    time_t time_inserted;
    bool isStopped;
    int time_up;
public:
    JobEntry(int job_id, std::string cmd_line, pid_t process_id, time_t time_inserted, bool isStopped, int time_up);
    ~JobEntry();
    void printJob(Command* cmd, int IO_status);
    int getJobID();
    pid_t getProcessID();
    time_t getTImeInserted();
    int getTimeUp();
    bool isStoppedProcess();
    void setIsStopped(bool setStopped);
    std::string getCmdLine();
};

class JobsList {
    std::vector<JobEntry>* jobs_vec;
    int max_job_id;
    int max_stopped_jod_id;
public:
    JobsList();
    ~JobsList();
    void addJob(int job_id, const char* cmd_line, pid_t pid, bool isStopped = false);
    void printJobsList(Command* cmd, int IO_status);
    void removeFinishedJobs();
    void updateMaxJobID();
    void updateMaxStoppedJobID();
    std::vector<JobEntry>* getJobsVec();
    JobEntry* getJobById(int jobId);
    JobEntry* getJobByProcessId(pid_t process_id);
    void removeJobByProcessId(pid_t process_to_delete);
    JobEntry* getLastStoppedJob();
    bool isVecEmpty();
    int getMaxJobID();
    int getMaxStoppedJobID();
    void turnToForeground(JobEntry* bg_or_stopped_job, Command* cmd, SmallShell* smash);
    void resumesStoppedJob(JobEntry* stopped_job, Command* cmd);
    void killAllJobs(Command* cmd);
};

class Command {
protected:
    const char* cmd_line;
    char* cmd_line_without_const;
    char* args[COMMAND_MAX_ARGS];
    std::string file_name;
    int IO_status;
    int args_length;
    bool is_time_out;
    int time_arg;
public:
    Command(const char* cmd_line);
    const char* getCmdLine();
    int getIOStatus();
    void ChangeIO(int isAppend, const char* buff, int length);
    virtual ~Command();
    virtual void execute() = 0;
    //virtual void prepare();
    //virtual void cleanup();
    // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
public:
    BuiltInCommand(const char* cmd_line);
    virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
    JobsList* jobs;
public:
    ExternalCommand(const char* cmd_line, JobsList* jobs);
    virtual ~ExternalCommand() {}
    void execute() override;
};

class PipeCommand : public Command {
    // TODO: Add your data members
public:
    PipeCommand(const char* cmd_line);
    virtual ~PipeCommand() {}
    void execute() override;
};

class RedirectionCommand : public Command {
    // TODO: Add your data members
public:
    explicit RedirectionCommand(const char* cmd_line);
    virtual ~RedirectionCommand() {}
    void execute() override;
    //void prepare() override;
    //void cleanup() override;
};

class SmallShell;
class ChangePromptCommand : public BuiltInCommand {
    SmallShell* smash;
public:
    ChangePromptCommand(const char* cmd_line, SmallShell* smash);
    virtual ~ChangePromptCommand() {}
    void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
    SmallShell* smash;
public:
    ChangeDirCommand(const char* cmd_line, SmallShell* smash);
    virtual ~ChangeDirCommand() {}
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line);
    virtual ~GetCurrDirCommand() {}
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
    SmallShell* smash;
public:
    ShowPidCommand(const char* cmd_line, SmallShell* smash);
    virtual ~ShowPidCommand() {}
    void execute() override;
};

class QuitCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    QuitCommand(const char* cmd_line, JobsList* jobs);
    virtual ~QuitCommand() {}
    void execute() override;
};

class JobsCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    JobsCommand(const char* cmd_line, JobsList* jobs);
    virtual ~JobsCommand() {}
    void execute() override;
};

class KillCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    KillCommand(const char* cmd_line, JobsList* jobs);
    virtual ~KillCommand() {}
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    JobsList* jobs;
    SmallShell* smash;
public:
    ForegroundCommand(const char* cmd_line, JobsList* jobs, SmallShell* smash);
    virtual ~ForegroundCommand() {}
    void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    BackgroundCommand(const char* cmd_line, JobsList* jobs);
    virtual ~BackgroundCommand() {}
    void execute() override;
};

class HeadCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    HeadCommand(const char* cmd_line, JobsList* jobs);
    virtual ~HeadCommand() {}
    void execute() override;
};

class SmallShell {
private:
    JobsList jobs_list;
    std::vector<JobEntry> time_jobs_vec;
    std::string prompt;
    char* last_pwd;
    bool lastPwdInitialized;
    int curr_job_id;
    std::string last_cmd;
    std::string curr_cmd_line;
    pid_t curr_process_id;
    pid_t smash_pid;
    SmallShell();
public:
    Command *CreateCommand(const char* cmd_line);
    JobsList* getJobsList();
    const char* getPrompt();
    char* getLastPwd();
    const char* getLastCmd();
    void setLastCmd(const char* cmd_line);
    int getCurrJobID();
    int getCurrProcessID();
    int getSmashPid();
    std::string getCurrCmdLine();
    int findMinAlarm();
    std::vector<JobEntry>* getTimeJobVec();
    bool isLastPwdInitialized();
    void setPrompt(std::string prompt);
    void setLastPwd(const char* last_pwd);
    void setCurrJobID(int job_id);
    void setCurrProcessID(int pid);
    void setCurrCmdLine(std::string cmd_line);
    void changeLastPwdStatus();
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell();
    void executeCommand(const char* cmd_line);
    // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_