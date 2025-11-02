export const API_BASE_URL = "http://localhost:8080/api";

export interface Activity {
  id: number;
  title: string;
  description?: string;
  scheduledTime: string;
  duration: number;
  priority?: number;
  completed: boolean;
  createdAt?: string;
  updatedAt?: string;
}

export interface Alarm {
  id: number;
  activityId: number;
  alarmTime: string;
  isActive?: boolean;
}
