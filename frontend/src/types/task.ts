export interface Task {
  id: string;
  title: string;
  description: string;
  dueDate: string;
  completed: boolean;
  priority: "low" | "medium" | "high";
  createdAt: string;
}

export interface QtBridge {
  addTask: (task: Omit<Task, "id" | "createdAt">) => void;
  updateTask: (id: string, task: Partial<Task>) => void;
  deleteTask: (id: string) => void;
  getTasks: () => void;
  markComplete: (id: string, completed: boolean) => void;
}

declare global {
  interface Window {
    qtBridge?: QtBridge;
    receiveTasksFromQt?: (tasks: Task[]) => void;
  }
}
