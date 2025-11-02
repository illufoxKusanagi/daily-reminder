// API helper for Qt backend activities
const API_BASE = "http://localhost:8080/api";

export interface Activity {
  id?: number;
  title: string;
  description: string;
  date: string; // YYYY-MM-DD
  time: string; // HH:MM
  completed?: boolean;
  category?: string;
  created_at?: string;
}

// ============================================
// FETCH (GET) - Get all activities
// ============================================
export async function fetchActivities(): Promise<Activity[]> {
  const response = await fetch(`${API_BASE}/activities`);
  if (!response.ok) throw new Error("Failed to fetch activities");
  return response.json();
}

// ============================================
// FETCH (GET) - Get single activity
// ============================================
export async function fetchActivity(id: number): Promise<Activity> {
  const response = await fetch(`${API_BASE}/activities/${id}`);
  if (!response.ok) throw new Error("Failed to fetch activity");
  return response.json();
}

// ============================================
// POST - Create new activity
// ============================================
export async function createActivity(
  data: Omit<Activity, "id" | "created_at">
): Promise<Activity> {
  const response = await fetch(`${API_BASE}/activities`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(data),
  });

  if (!response.ok) {
    const error = await response.json();
    throw new Error(error.error || "Failed to create activity");
  }

  return response.json();
}

// ============================================
// PUT - Update activity
// ============================================
export async function updateActivity(
  id: number,
  data: Partial<Activity>
): Promise<Activity> {
  const response = await fetch(`${API_BASE}/activities/${id}`, {
    method: "PUT",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(data),
  });

  if (!response.ok) {
    const error = await response.json();
    throw new Error(error.error || "Failed to update activity");
  }

  return response.json();
}

// ============================================
// DELETE - Delete activity
// ============================================
export async function deleteActivity(id: number): Promise<void> {
  const response = await fetch(`${API_BASE}/activities/${id}`, {
    method: "DELETE",
  });

  if (!response.ok) {
    const error = await response.json();
    throw new Error(error.error || "Failed to delete activity");
  }
}

// ============================================
// PATCH - Mark as completed
// ============================================
export async function toggleActivityCompletion(
  id: number,
  completed: boolean
): Promise<void> {
  const response = await fetch(`${API_BASE}/activities/${id}/complete`, {
    method: "PATCH",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ completed }),
  });

  if (!response.ok) {
    const error = await response.json();
    throw new Error(error.error || "Failed to update activity");
  }
}

// ============================================
// USAGE EXAMPLES:
// ============================================

/*
// 1. FETCH all activities
const activities = await fetchActivities();
console.log(activities);

// 2. CREATE new activity
const newActivity = await createActivity({
  title: "Morning Exercise",
  description: "30 minutes workout",
  date: "2025-11-03",
  time: "06:00",
  category: "Health",
  completed: false,
});
console.log("Created:", newActivity);

// 3. UPDATE activity
const updated = await updateActivity(1, {
  title: "Updated Title",
  description: "Updated description",
});
console.log("Updated:", updated);

// 4. DELETE activity
await deleteActivity(1);
console.log("Deleted!");

// 5. TOGGLE completion
await toggleActivityCompletion(1, true);
console.log("Marked as completed!");
*/
