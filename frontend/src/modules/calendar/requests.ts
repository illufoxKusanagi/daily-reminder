import type { IEvent } from "@/modules/calendar/interfaces";

// Use direct backend URL
const API_BASE_URL = "http://localhost:8080";

export const getEvents = async (): Promise<IEvent[]> => {
  const response = await fetch(`${API_BASE_URL}/api/event`);
  if (!response.ok) throw new Error("Failed to fetch events");
  return response.json();
};

export const createEvent = async (
  event: Omit<IEvent, "id">
): Promise<IEvent> => {
  const response = await fetch(`${API_BASE_URL}/api/event`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(event),
  });
  if (!response.ok) throw new Error("Failed to create event");
  return response.json();
};

export const updateEvent = async (
  id: string,
  event: Partial<IEvent>
): Promise<IEvent> => {
  const response = await fetch(`${API_BASE_URL}/api/event/${id}`, {
    method: "PUT",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(event),
  });
  if (!response.ok) throw new Error("Failed to update event");
  return response.json();
};

export const deleteEvent = async (id: string): Promise<void> => {
  const response = await fetch(`${API_BASE_URL}/api/event/${id}`, {
    method: "DELETE",
  });
  if (!response.ok) throw new Error("Failed to delete event");
};

export const getUsers = async () => {
  return [];
};
