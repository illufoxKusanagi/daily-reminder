import { Activity, Alarm, API_BASE_URL } from "@/types/backend-props";

class ApiClient {
  private async request<T>(
    endpoint: string,
    options?: RequestInit
  ): Promise<T> {
    const response = await fetch(`${API_BASE_URL}${endpoint}`, {
      ...options,
      headers: {
        "Content-Type": "application/json",
        ...options?.headers,
      },
    });

    if (!response.ok) {
      const error = await response.json();
      throw new Error(error.error || "API request failed");
    }

    return response.json();
  }

  // Activity endpoints
  async getActivities(): Promise<Activity[]> {
    return this.request("/activities");
  }

  async getActivity(id: number): Promise<Activity> {
    return this.request(`/activities/${id}`);
  }

  async createActivity(
    activity: Omit<Activity, "id" | "createdAt" | "updatedAt">
  ): Promise<Activity> {
    return this.request("/activities", {
      method: "POST",
      body: JSON.stringify(activity),
    });
  }

  async updateActivity(
    id: number,
    activity: Partial<Activity>
  ): Promise<Activity> {
    return this.request(`/activities/${id}`, {
      method: "PUT",
      body: JSON.stringify(activity),
    });
  }

  async deleteActivity(id: number): Promise<void> {
    await this.request(`/activities/${id}`, { method: "DELETE" });
  }

  async markCompleted(id: number, completed: boolean): Promise<void> {
    await this.request(`/activities/${id}/complete`, {
      method: "PATCH",
      body: JSON.stringify({ completed }),
    });
  }

  async getUpcomingActivities(): Promise<Activity[]> {
    return this.request("/activities/upcoming");
  }

  // Alarm endpoints
  async createAlarm(alarm: Omit<Alarm, "id" | "isActive">): Promise<Alarm> {
    return this.request("/alarms", {
      method: "POST",
      body: JSON.stringify(alarm),
    });
  }

  async getAlarmsForActivity(activityId: number): Promise<Alarm[]> {
    return this.request(`/alarms/activity/${activityId}`);
  }

  async deleteAlarm(id: number): Promise<void> {
    await this.request(`/alarms/${id}`, { method: "DELETE" });
  }
}

export const api = new ApiClient();
