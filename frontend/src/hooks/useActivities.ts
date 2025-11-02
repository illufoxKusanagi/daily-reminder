"use client";

import { useState, useEffect } from "react";
import { api, Activity } from "@/lib/api";

export function useActivities() {
  const [activities, setActivities] = useState<Activity[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  const fetchActivities = async () => {
    try {
      setLoading(true);
      const data = await api.getActivities();
      setActivities(data);
      setError(null);
    } catch (err) {
      setError(
        err instanceof Error ? err.message : "Failed to fetch activities"
      );
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchActivities();
  }, []);

  const addActivity = async (
    activity: Omit<Activity, "id" | "createdAt" | "updatedAt">
  ) => {
    try {
      const newActivity = await api.createActivity(activity);
      setActivities([...activities, newActivity]);
      return newActivity;
    } catch (err) {
      setError(
        err instanceof Error ? err.message : "Failed to create activity"
      );
      throw err;
    }
  };

  const updateActivity = async (id: number, updates: Partial<Activity>) => {
    try {
      const updated = await api.updateActivity(id, updates);
      setActivities(activities.map((a) => (a.id === id ? updated : a)));
      return updated;
    } catch (err) {
      setError(
        err instanceof Error ? err.message : "Failed to update activity"
      );
      throw err;
    }
  };

  const deleteActivity = async (id: number) => {
    try {
      await api.deleteActivity(id);
      setActivities(activities.filter((a) => a.id !== id));
    } catch (err) {
      setError(
        err instanceof Error ? err.message : "Failed to delete activity"
      );
      throw err;
    }
  };

  const markCompleted = async (id: number, completed: boolean) => {
    try {
      await api.markCompleted(id, completed);
      setActivities(
        activities.map((a) => (a.id === id ? { ...a, completed } : a))
      );
    } catch (err) {
      setError(
        err instanceof Error ? err.message : "Failed to update activity"
      );
      throw err;
    }
  };

  return {
    activities,
    loading,
    error,
    addActivity,
    updateActivity,
    deleteActivity,
    markCompleted,
    refresh: fetchActivities,
  };
}
